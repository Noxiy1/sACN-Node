/*
 * sACN → DMX512 Konverter
 * Hardware: WT32-ETH01 + 3× MAX485 + SSD1306 OLED 128×32
 *
 * ┌─────────────────────────────────────────────────────┐
 * │  PIN-BELEGUNG                                       │
 * ├──────────────┬────────┬───────────────────────────  │
 * │  Funktion    │  GPIO  │  Anmerkung                  │
 * ├──────────────┼────────┼───────────────────────────  │
 * │  OLED SDA    │   4    │  I²C                        │
 * │  OLED SCL    │   2    │  I²C                        │
 * ├──────────────┼────────┼───────────────────────────  │
 * │  DMX1 TX     │  14    │  UART2 TX                   │
 * │  DMX1 DE/RE  │  15    │  gebrückt auf einem Pin     │
 * ├──────────────┼────────┼───────────────────────────  │
 * │  DMX2 TX     │  12    │  UART1 TX                   │
 * │  DMX2 DE/RE  │  13    │  gebrückt auf einem Pin     │
 * ├──────────────┼────────┼───────────────────────────  │
 * │  DMX3 TX     │  33    │  UART0 TX (remapped!)       │
 * │  DMX3 DE/RE  │  17    │  gebrückt auf einem Pin     │
 * ├──────────────┼────────┼───────────────────────────  │
 * │  Debug RX/TX │  1/3   │  UART0 default (USB-Serial) │
 * └──────────────┴────────┴───────────────────────────  ┘
 *
 * UART0 wird auf GPIO33 remappt → Debug-Serial (GPIO1/3) bleibt frei.
 * RX aller DMX-UARTs → GPIO36 (Input-Only, nie genutzt).
 *
 * Bibliotheken (Arduino Library Manager):
 *   - Adafruit SSD1306
 *   - Adafruit GFX Library
 *   ESP32 Arduino Core bringt ETH.h mit.
 *
 * Board: "WT32-ETH01" oder "ESP32 Dev Module", 240 MHz
 */

#include <Arduino.h>
#include <ETH.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ─────────────────────────────────────────────────────
//  PIN-DEFINITIONEN
// ─────────────────────────────────────────────────────

#define OLED_SDA      4
#define OLED_SCL      2

// Gemeinsamer Dummy-RX (Input-Only, nie genutzt)
#define DMX_RX_DUMMY  36

// Port 1 – UART2
#define DMX1_TX       14
#define DMX1_DE_RE    15

// Port 2 – UART1
#define DMX2_TX       12
#define DMX2_DE_RE    13

// Port 3 – UART0 remappt auf GPIO33 (Debug-Serial bleibt auf GPIO1/3)
#define DMX3_TX       33
#define DMX3_DE_RE    17

// ─────────────────────────────────────────────────────
//  KONFIGURATION
// ─────────────────────────────────────────────────────

#define UNIVERSE_1        1
#define UNIVERSE_2        2
#define UNIVERSE_3        3

#define DMX_BAUD      250000
#define DMX_BREAK_US    176    // >= 88 µs   (Spec: min 88, typ 176)
#define DMX_MAB_US       12    // >= 8 µs
#define DMX_CHANNELS    512

#define SACN_PORT        5568
#define SACN_TIMEOUT_MS  3000

#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT  32
#define OLED_RESET     -1
#define OLED_I2C_ADDR  0x3C

// ─────────────────────────────────────────────────────
//  GLOBALE OBJEKTE
// ─────────────────────────────────────────────────────

// UART2, UART1, UART0 – alle HardwareSerial, kein SoftwareSerial
HardwareSerial dmxSerial1(2);   // UART2 → DMX Port 1
HardwareSerial dmxSerial2(1);   // UART1 → DMX Port 2
HardwareSerial dmxSerial3(0);   // UART0 → DMX Port 3 (remappt)

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

WiFiUDP udp;

// DMX-Datenpuffer: [Port][0..511], Byte 0 = Startcode 0x00
uint8_t dmxData[3][DMX_CHANNELS + 1];

bool          sacnActive[3]  = {false, false, false};
unsigned long lastPacket[3]  = {0, 0, 0};

bool   ethConnected = false;
String localIP      = "0.0.0.0";

// ─────────────────────────────────────────────────────
//  DMX BREAK + MAB + FRAME
//
//  Ablauf je Frame:
//    1. DE/RE HIGH (Sender aktiv)
//    2. TX-Pin manuell LOW  → BREAK
//    3. TX-Pin manuell HIGH → MAB
//    4. UART übernimmt den Pin, sendet Startcode + 512 Byte
//    5. serial.flush() wartet bis FIFO leer
// ─────────────────────────────────────────────────────

// Hilfsfunktion: UART-Pin manuell auf LOW/HIGH schalten,
// bevor der UART die Kontrolle zurückbekommt.
static inline void pinLow(uint8_t pin) {
  gpio_set_direction((gpio_num_t)pin, GPIO_MODE_OUTPUT);
  gpio_set_level((gpio_num_t)pin, 0);
}
static inline void pinHigh(uint8_t pin) {
  gpio_set_level((gpio_num_t)pin, 1);
}

void sendDmxFrame(HardwareSerial& ser, uint8_t txPin, uint8_t deRePin,
                  uint8_t* data) {
  // --- Break ---
  digitalWrite(deRePin, HIGH);   // Sender einschalten
  pinLow(txPin);
  delayMicroseconds(DMX_BREAK_US);

  // --- MAB ---
  pinHigh(txPin);
  delayMicroseconds(DMX_MAB_US);

  // --- Daten senden ---
  // Der UART hat den Pin bereits während begin() als UART_TX konfiguriert;
  // nach dem manuellen LOW/HIGH übernimmt er sofort wieder.
  ser.write(data, DMX_CHANNELS + 1);   // Startcode (data[0]=0x00) + 512 Byte
  ser.flush();                          // blockiert bis FIFO komplett gesendet
}

// ─────────────────────────────────────────────────────
//  sACN E1.31 PARSER
// ─────────────────────────────────────────────────────

static const uint8_t ACN_ID[12] = {
  0x41,0x53,0x43,0x2d,0x45,0x31,0x2e,0x31,
  0x37,0x00,0x00,0x00
};

struct SacnPacket {
  bool      valid;
  uint16_t  universe;
  uint8_t   priority;
  uint16_t  dmxLen;
  uint8_t*  payload;   // Zeiger auf Startcode-Byte im UDP-Puffer
};

SacnPacket parseSacn(uint8_t* buf, int len) {
  SacnPacket pkt = {false, 0, 0, 0, nullptr};
  if (len < 126) return pkt;

  // Preamble Size @ 0: 0x00 0x10
  if (buf[0] != 0x00 || buf[1] != 0x10) return pkt;

  // ACN Identifier @ 4
  if (memcmp(buf + 4, ACN_ID, 12) != 0) return pkt;

  // Root Vector @ 18: 0x00000004
  uint32_t rv = ((uint32_t)buf[18] << 24) | ((uint32_t)buf[19] << 16)
              | ((uint32_t)buf[20] <<  8) |  buf[21];
  if (rv != 0x00000004ul) return pkt;

  // Framing Vector @ 40: 0x00000002
  uint32_t fv = ((uint32_t)buf[40] << 24) | ((uint32_t)buf[41] << 16)
              | ((uint32_t)buf[42] <<  8) |  buf[43];
  if (fv != 0x00000002ul) return pkt;

  // DMP Vector @ 117: 0x02
  if (buf[117] != 0x02) return pkt;

  pkt.valid    = true;
  pkt.universe = ((uint16_t)buf[113] << 8) | buf[114];
  pkt.priority = buf[108];
  // Property count @ 123-124, minus Startcode-Byte
  pkt.dmxLen   = (uint16_t)(((buf[123] & 0x0F) << 8) | buf[124]) - 1u;
  pkt.payload  = buf + 125;   // Startcode @ 125, DMX-Daten ab 126

  return pkt;
}

// ─────────────────────────────────────────────────────
//  OLED
// ─────────────────────────────────────────────────────

void updateOled() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Zeile 0 (y=0): IP
  display.setCursor(0, 0);
  display.print("IP: ");
  display.print(localIP);

  // Zeile 1 (y=11): Universum-Status
  display.setCursor(0, 11);
  for (int i = 0; i < 3; i++) {
    display.print("U");
    display.print(i + 1);
    display.print(":");
    display.print(sacnActive[i] ? "OK" : "--");
    if (i < 2) display.print(" ");
  }

  // Zeile 2 (y=22): ETH
  display.setCursor(0, 22);
  display.print(ethConnected ? "ETH: verbunden" : "ETH: getrennt ");

  display.display();
}

// ─────────────────────────────────────────────────────
//  MULTICAST
// ─────────────────────────────────────────────────────

IPAddress sacnGroup(uint16_t uni) {
  return IPAddress(239, 255, (uni >> 8) & 0xFF, uni & 0xFF);
}

void joinMulticast() {
  // Erste Gruppe über WiFiUDP
  udp.beginMulticast(sacnGroup(UNIVERSE_1), SACN_PORT);

  // Weitere Gruppen direkt über LwIP IGMP
  struct netif* iface = netif_default;
  if (!iface) return;

  ip4_addr_t g2, g3;
  IPAddress m2 = sacnGroup(UNIVERSE_2);
  IPAddress m3 = sacnGroup(UNIVERSE_3);
  IP4_ADDR(&g2, m2[0], m2[1], m2[2], m2[3]);
  IP4_ADDR(&g3, m3[0], m3[1], m3[2], m3[3]);
  igmp_joingroup_netif(iface, &g2);
  igmp_joingroup_netif(iface, &g3);
}

// ─────────────────────────────────────────────────────
//  ETHERNET-EREIGNISSE
// ─────────────────────────────────────────────────────

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

// ─────────────────────────────────────────────────────
//  SETUP
// ─────────────────────────────────────────────────────

void setup() {
  // Debug-Serial auf UART0-Default-Pins (GPIO1 TX, GPIO3 RX)
  // UART0 für DMX3 wird auf GPIO33 remappt – kein Konflikt.
  Serial.begin(115200);
  Serial.println("sACN→DMX booting...");

  // DE/RE initial LOW (Empfangsmodus, Ausgänge still)
  pinMode(DMX1_DE_RE, OUTPUT); digitalWrite(DMX1_DE_RE, LOW);
  pinMode(DMX2_DE_RE, OUTPUT); digitalWrite(DMX2_DE_RE, LOW);
  pinMode(DMX3_DE_RE, OUTPUT); digitalWrite(DMX3_DE_RE, LOW);

  // DMX-Puffer auf 0 (Blackout)
  memset(dmxData, 0, sizeof(dmxData));

  // UART2 → DMX Port 1 (TX GPIO14, RX Dummy GPIO36)
  dmxSerial1.begin(DMX_BAUD, SERIAL_8N2, DMX_RX_DUMMY, DMX1_TX);

  // UART1 → DMX Port 2 (TX GPIO12, RX Dummy GPIO36)
  dmxSerial2.begin(DMX_BAUD, SERIAL_8N2, DMX_RX_DUMMY, DMX2_TX);

  // UART0 → DMX Port 3 (TX GPIO33 remappt, RX Dummy GPIO36)
  // UART0 default-Pins werden durch begin() mit expliziten Pins überschrieben.
  dmxSerial3.begin(DMX_BAUD, SERIAL_8N2, DMX_RX_DUMMY, DMX3_TX);

  // OLED
  Wire.begin(OLED_SDA, OLED_SCL);
  if (display.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDR)) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);  display.print("sACN -> DMX");
    display.setCursor(0, 11); display.print("3x Universum");
    display.setCursor(0, 22); display.print("Warte auf ETH...");
    display.display();
  }

  // Ethernet
  Network.onEvent(onEthEvent);
  ETH.begin();
}

// ─────────────────────────────────────────────────────
//  LOOP
// ─────────────────────────────────────────────────────

static uint8_t       udpBuf[638];
static unsigned long lastOledUpdate = 0;
static unsigned long lastDmxSend    = 0;

void loop() {
  unsigned long now = millis();

  // ── UDP empfangen ──────────────────────────────────
  int pktLen = udp.parsePacket();
  if (pktLen > 0) {
    int n = udp.read(udpBuf, sizeof(udpBuf));
    if (n > 0) {
      SacnPacket pkt = parseSacn(udpBuf, n);
      if (pkt.valid) {
        int uIdx = -1;
        if      (pkt.universe == UNIVERSE_1) uIdx = 0;
        else if (pkt.universe == UNIVERSE_2) uIdx = 1;
        else if (pkt.universe == UNIVERSE_3) uIdx = 2;

        if (uIdx >= 0 && pkt.dmxLen > 0) {
          dmxData[uIdx][0] = 0x00;  // DMX Startcode
          uint16_t copyLen = min((int)pkt.dmxLen, DMX_CHANNELS);
          memcpy(dmxData[uIdx] + 1, pkt.payload + 1, copyLen);
          // Rest auf 0, falls Paket kürzer als 512
          if (copyLen < DMX_CHANNELS)
            memset(dmxData[uIdx] + 1 + copyLen, 0, DMX_CHANNELS - copyLen);
          lastPacket[uIdx] = now;
          sacnActive[uIdx] = true;
        }
      }
    }
  }

  // ── Timeout-Check ──────────────────────────────────
  for (int i = 0; i < 3; i++) {
    if (sacnActive[i] && (now - lastPacket[i] > SACN_TIMEOUT_MS)) {
      sacnActive[i] = false;
      memset(dmxData[i], 0, sizeof(dmxData[i]));
      Serial.printf("Universum %d: Signal verloren\n", i + 1);
    }
  }

  // ── DMX senden (~44 Hz) ────────────────────────────
  if (now - lastDmxSend >= 23) {
    lastDmxSend = now;
    sendDmxFrame(dmxSerial1, DMX1_TX, DMX1_DE_RE, dmxData[0]);
    sendDmxFrame(dmxSerial2, DMX2_TX, DMX2_DE_RE, dmxData[1]);
    sendDmxFrame(dmxSerial3, DMX3_TX, DMX3_DE_RE, dmxData[2]);
  }

  // ── OLED-Update (1 Hz) ─────────────────────────────
  if (now - lastOledUpdate >= 1000) {
    lastOledUpdate = now;
    updateOled();
  }
}
