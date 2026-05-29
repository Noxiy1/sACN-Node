/*
 * sACN → DMX512 Konverter
 * Hardware: WT32-ETH01 + 3× MAX485 + SSD1306 OLED 128×32
 *
 * PIN-Belegung:
 *   OLED SDA      → GPIO 4
 *   OLED SCL      → GPIO 2
 *   MAX485 #1 TX  → GPIO 14   (UART2)
 *   MAX485 #1 DE/RE→ GPIO 15
 *   MAX485 #2 TX  → GPIO 12   (UART via SoftwareSerial / HardwareSerial)
 *   MAX485 #2 DE/RE→ GPIO 13
 *   MAX485 #3 TX  → GPIO 5    (UART1)
 *   MAX485 #3 DE/RE→ GPIO 17
 *
 * Bibliotheken (Arduino Library Manager):
 *   - ETH (enthalten in ESP32 Arduino Core)
 *   - Adafruit_SSD1306
 *   - Adafruit_GFX
 *
 * sACN: E1.31 UDP Multicast, Port 5568
 *
 * Kompilieren: Board = "WT32-ETH01" (oder "ESP32 Dev Module")
 */

#include <Arduino.h>
#include <ETH.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ──────────────────────────────────────────
//  KONFIGURATION
// ──────────────────────────────────────────

// sACN-Universen (1-basiert)
#define UNIVERSE_1  1
#define UNIVERSE_2  2
#define UNIVERSE_3  3

// GPIO-Definitionen
#define OLED_SDA    4
#define OLED_SCL    2

#define DMX1_TX     14
#define DMX1_DE_RE  15

#define DMX2_TX     12
#define DMX2_DE_RE  13

#define DMX3_TX     5
#define DMX3_DE_RE  17

// DMX-Timing (µs)
#define DMX_BREAK_US    176   // >= 88 µs
#define DMX_MAB_US       16   // >= 8 µs
#define DMX_CHANNELS    512

// sACN
#define SACN_PORT       5568
#define SACN_TIMEOUT_MS 3000   // Signal als verloren markieren nach 3 s

// OLED
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT  32
#define OLED_RESET     -1

// ──────────────────────────────────────────
//  GLOBALE VARIABLEN
// ──────────────────────────────────────────

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

WiFiUDP udp;

uint8_t dmxData[3][DMX_CHANNELS + 1]; // Index 0 = Startcode (0x00)

bool  sacnActive[3]    = {false, false, false};
unsigned long lastPacket[3] = {0, 0, 0};

bool  ethConnected = false;
String localIP     = "0.0.0.0";

// HardwareSerial für 3 DMX-Ports
// UART0 = Serial (Debug), UART1, UART2 verfügbar
HardwareSerial dmxSerial1(2); // UART2 → TX GPIO14
HardwareSerial dmxSerial2(1); // UART1 → TX GPIO12  (RX dummy = 0)
HardwareSerial dmxSerial3(0); // UART0 → TX GPIO5   (RX dummy = 0, kein Debug-Output!)

// ──────────────────────────────────────────
//  sACN E1.31 PAKET-PARSE
// ──────────────────────────────────────────

// sACN ACN Packet Identifier
static const uint8_t ACN_ID[] = {
  0x41, 0x53, 0x43, 0x2d, 0x45, 0x31, 0x2e, 0x31,
  0x37, 0x00, 0x00, 0x00
};

struct SacnPacket {
  bool    valid;
  uint16_t universe;
  uint8_t  priority;
  uint16_t dmxLen;
  uint8_t* dmxData; // Zeiger in den UDP-Puffer (inkl. Startcode)
};

SacnPacket parseSacn(uint8_t* buf, int len) {
  SacnPacket pkt = {false, 0, 0, 0, nullptr};
  if (len < 126) return pkt;

  // Preamble Size: Offset 0, Wert 0x0010
  if (buf[0] != 0x00 || buf[1] != 0x10) return pkt;

  // ACN Identifier: Offset 4..15
  for (int i = 0; i < 12; i++) {
    if (buf[4 + i] != ACN_ID[i]) return pkt;
  }

  // Vektoren prüfen (Root = 0x00000004, Framing = 0x00000002, DMP = 0x02)
  uint32_t rootVector = ((uint32_t)buf[18] << 24) | ((uint32_t)buf[19] << 16) |
                        ((uint32_t)buf[20] << 8)  | buf[21];
  if (rootVector != 0x00000004) return pkt;

  uint32_t framingVector = ((uint32_t)buf[40] << 24) | ((uint32_t)buf[41] << 16) |
                           ((uint32_t)buf[42] << 8)  | buf[43];
  if (framingVector != 0x00000002) return pkt;

  if (buf[117] != 0x02) return pkt; // DMP Vector

  pkt.valid    = true;
  pkt.universe = ((uint16_t)buf[113] << 8) | buf[114];
  pkt.priority = buf[108];
  pkt.dmxLen   = (((uint16_t)(buf[123] & 0x0F) << 8) | buf[124]) - 1;
  pkt.dmxData  = buf + 126; // Startcode + 512 Byte DMX

  return pkt;
}

// ──────────────────────────────────────────
//  DMX AUSGABE
// ──────────────────────────────────────────

void sendDmx(HardwareSerial& serial, int deRePin, uint8_t* data, uint16_t len) {
  // DMX Break: Leitung auf LOW ziehen (über serielle Stop-Bits simuliert)
  // Zuverlässigster Weg auf ESP32: baudrate wechseln für Break
  serial.end();
  pinMode(DMX1_TX == deRePin - 1 ? DMX1_TX :
          DMX2_TX == deRePin - 1 ? DMX2_TX : DMX3_TX, OUTPUT);
  // DE/RE HIGH = Senden
  digitalWrite(deRePin, HIGH);

  // BREAK
  int txPin = (deRePin == DMX1_DE_RE) ? DMX1_TX :
              (deRePin == DMX2_DE_RE) ? DMX2_TX : DMX3_TX;
  digitalWrite(txPin, LOW);
  delayMicroseconds(DMX_BREAK_US);
  // MAB
  digitalWrite(txPin, HIGH);
  delayMicroseconds(DMX_MAB_US);

  // DMX mit 250 kBaud senden
  if (deRePin == DMX1_DE_RE)
    serial.begin(250000, SERIAL_8N2, -1, DMX1_TX);
  else if (deRePin == DMX2_DE_RE)
    serial.begin(250000, SERIAL_8N2, -1, DMX2_TX);
  else
    serial.begin(250000, SERIAL_8N2, -1, DMX3_TX);

  // Startcode + Daten
  serial.write((uint8_t)0x00); // Startcode
  serial.write(data + 1, min((int)len, DMX_CHANNELS));
  serial.flush();

  // DE/RE LOW = Empfang (nicht nötig, aber sauber)
  // digitalWrite(deRePin, LOW);
}

// Wrapper für jeden Port
void sendDmxPort1() {
  sendDmx(dmxSerial1, DMX1_DE_RE, dmxData[0], DMX_CHANNELS);
}
void sendDmxPort2() {
  sendDmx(dmxSerial2, DMX2_DE_RE, dmxData[1], DMX_CHANNELS);
}
void sendDmxPort3() {
  sendDmx(dmxSerial3, DMX3_DE_RE, dmxData[2], DMX_CHANNELS);
}

// ──────────────────────────────────────────
//  OLED ANZEIGE
// ──────────────────────────────────────────

void updateOled() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Zeile 1: IP-Adresse
  display.setCursor(0, 0);
  display.print("IP:");
  display.print(localIP);

  // Zeile 2: sACN-Status je Universum
  display.setCursor(0, 12);
  display.print("U");
  display.print(UNIVERSE_1);
  display.print(":");
  display.print(sacnActive[0] ? "OK" : "--");

  display.print("  U");
  display.print(UNIVERSE_2);
  display.print(":");
  display.print(sacnActive[1] ? "OK" : "--");

  display.print("  U");
  display.print(UNIVERSE_3);
  display.print(":");
  display.print(sacnActive[2] ? "OK" : "--");

  // Zeile 3: ETH-Status
  display.setCursor(0, 24);
  display.print(ethConnected ? "ETH: verbunden" : "ETH: getrennt");

  display.display();
}

// ──────────────────────────────────────────
//  MULTICAST BEITRITT
// ──────────────────────────────────────────

IPAddress sacnMulticast(uint16_t universe) {
  // sACN Multicast: 239.255.x.x (x = Universe high/low byte)
  return IPAddress(239, 255, (universe >> 8) & 0xFF, universe & 0xFF);
}

void joinMulticast() {
  IPAddress mc1 = sacnMulticast(UNIVERSE_1);
  IPAddress mc2 = sacnMulticast(UNIVERSE_2);
  IPAddress mc3 = sacnMulticast(UNIVERSE_3);

  ip_addr_t addr1, addr2, addr3;
  addr1.addr = mc1;
  addr2.addr = mc2;
  addr3.addr = mc3;

  udp.beginMulticast(mc1, SACN_PORT);
  // Weitere Gruppen über LwIP direkt beitreten
  // (WiFiUDP unterstützt nur eine Gruppe – raw LwIP für die anderen)
  struct netif* netif = netif_default;
  if (netif) {
    igmp_joingroup_netif(netif, (ip4_addr_t*)&addr2.addr);
    igmp_joingroup_netif(netif, (ip4_addr_t*)&addr3.addr);
  }
}

// ──────────────────────────────────────────
//  ETHERNET-EREIGNISSE
// ──────────────────────────────────────────

void onEthEvent(arduino_event_id_t event) {
  switch (event) {
    case ARDUINO_EVENT_ETH_START:
      ETH.setHostname("sacn-dmx");
      break;
    case ARDUINO_EVENT_ETH_CONNECTED:
      ethConnected = true;
      break;
    case ARDUINO_EVENT_ETH_GOT_IP:
      localIP = ETH.localIP().toString();
      joinMulticast();
      updateOled();
      break;
    case ARDUINO_EVENT_ETH_DISCONNECTED:
      ethConnected = false;
      localIP = "0.0.0.0";
      updateOled();
      break;
    default:
      break;
  }
}

// ──────────────────────────────────────────
//  SETUP
// ──────────────────────────────────────────

void setup() {
  // Serielle Debug-Ausgabe (nur wenn DMX3 nicht UART0 nutzt – hier via UART0 TX=GPIO5)
  // GPIO5 ist TX3, daher kein normaler Serial.begin() für Debug!
  // Optional: Serial.begin(115200) NUR für Tests ohne MAX485 #3.

  // DE/RE Pins als Ausgang, initial LOW (Empfangsmodus)
  pinMode(DMX1_DE_RE, OUTPUT); digitalWrite(DMX1_DE_RE, LOW);
  pinMode(DMX2_DE_RE, OUTPUT); digitalWrite(DMX2_DE_RE, LOW);
  pinMode(DMX3_DE_RE, OUTPUT); digitalWrite(DMX3_DE_RE, LOW);

  // DMX-Daten mit 0 initialisieren
  memset(dmxData, 0, sizeof(dmxData));

  // OLED initialisieren
  Wire.begin(OLED_SDA, OLED_SCL);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    // OLED nicht gefunden – weiter ohne Display
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("sACN -> DMX");
  display.setCursor(0, 12);
  display.print("Warte auf ETH...");
  display.display();

  // Ethernet starten
  Network.onEvent(onEthEvent);
  ETH.begin();
}

// ──────────────────────────────────────────
//  LOOP
// ──────────────────────────────────────────

static uint8_t udpBuf[638]; // max. sACN-Paketgröße
static unsigned long lastOledUpdate = 0;
static unsigned long lastDmxSend    = 0;

void loop() {
  unsigned long now = millis();

  // UDP-Pakete empfangen
  int pktLen = udp.parsePacket();
  if (pktLen > 0) {
    int read = udp.read(udpBuf, sizeof(udpBuf));
    if (read > 0) {
      SacnPacket pkt = parseSacn(udpBuf, read);
      if (pkt.valid) {
        int uIdx = -1;
        if      (pkt.universe == UNIVERSE_1) uIdx = 0;
        else if (pkt.universe == UNIVERSE_2) uIdx = 1;
        else if (pkt.universe == UNIVERSE_3) uIdx = 2;

        if (uIdx >= 0 && pkt.dmxLen > 0) {
          // Startcode + Daten übernehmen
          memcpy(dmxData[uIdx] + 1, pkt.dmxData + 1,
                 min((int)pkt.dmxLen, DMX_CHANNELS));
          dmxData[uIdx][0] = 0x00; // DMX Startcode
          lastPacket[uIdx]  = now;
          sacnActive[uIdx]  = true;
        }
      }
    }
  }

  // Timeout prüfen
  for (int i = 0; i < 3; i++) {
    if (sacnActive[i] && (now - lastPacket[i] > SACN_TIMEOUT_MS)) {
      sacnActive[i] = false;
      memset(dmxData[i], 0, sizeof(dmxData[i]));
    }
  }

  // DMX senden (ca. 44 Hz – alle ~23 ms)
  if (now - lastDmxSend >= 23) {
    lastDmxSend = now;
    sendDmxPort1();
    sendDmxPort2();
    sendDmxPort3();
  }

  // OLED aktualisieren (1× pro Sekunde)
  if (now - lastOledUpdate >= 1000) {
    lastOledUpdate = now;
    updateOled();
  }
}
