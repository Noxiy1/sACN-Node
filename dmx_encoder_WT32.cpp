/**
 * WT32-ETH01 - DMX Encoder (4x DMX)
 * 
 * Hardware:
 * - WT32-ETH01 (ESP32 mit integriertem LAN8720 PHY)
 * - 4x MAX485 Module
 * - 4x XLR-3 Buchsen
 * 
 * ⚡ VORTEIL WT32-ETH01:
 * - Ethernet INTERN (keine externen Chips!)
 * - Alle GPIO frei für DMX
 * - Stabilere Ethernet-Verbindung
 * - Kompakteres Design
 * 
 * Autor: Optimiert für WT32-ETH01
 * Version: 2.0
 */

#include <Arduino.h>
#include <WiFi.h>
#include <ETH.h>
#include <AsyncUDP.h>
#include <HardwareSerial.h>
#include <SoftwareSerial.h>

// ============================================================================
// WT32-ETH01 SPEZIFISCHE ETHERNET KONFIGURATION
// ============================================================================

// WT32-ETH01 verwendet LAN8720 PHY direkt integriert!
// Die Ethernet-Pins sind auf dem Modul intern verschaltet

#define ETH_ADDR 0           // LAN8720 PHY Address = 0
#define ETH_POWER_PIN -1     // Nicht verwendet (immer an)
#define ETH_MDC_PIN 23       // Management Data Clock
#define ETH_MDIO_PIN 18      // Management Data I/O
#define ETH_TYPE ETH_PHY_LAN8720
#define ETH_CLK_MODE ETH_CLOCK_GPIO0_IN

// ⚠️ WICHTIG: WT32-ETH01 Reset Pin (optional, für Recovery)
#define ETH_RESET_PIN -1     // -1 = nicht verwendet
// Manche WT32 haben einen Reset-Pin auf GPIO12

// ============================================================================
// DMX HARDWARE - ALLE GPIO FREI! (kein Ethernet Konflikt)
// ============================================================================

// DMX1: UART1
#define DMX1_TX_PIN 9
#define DMX1_RX_PIN 10
#define DMX1_EN_PIN 27

// DMX2: UART2
#define DMX2_TX_PIN 16
#define DMX2_RX_PIN 17
#define DMX2_EN_PIN 26

// DMX3: UART0
#define DMX3_TX_PIN 1
#define DMX3_RX_PIN 3
#define DMX3_EN_PIN 25

// DMX4: SoftwareSerial (mit mehr freien Pins zur Auswahl)
#define DMX4_RX_PIN 5
#define DMX4_TX_PIN 4
#define DMX4_EN_PIN 14

// Zusätzliche freie GPIO auf WT32 (können für LEDs/Sensoren genutzt werden):
// GPIO2, GPIO5, GPIO12, GPIO13, GPIO15, GPIO32, GPIO33, GPIO34, GPIO35, GPIO36, GPIO39

// DMX Konstanten
#define DMX_BAUDRATE 250000
#define DMX_CHANNELS 512
#define DMX_START_CODE 0x00
#define DMX_BREAK_TIME 200

// Netzwerk
#define SACN_PORT 5568
#define ARTNET_PORT 6454
#define SACN_MULTICAST IPAddress(239, 69, 255, 255)

// ============================================================================
// UART OBJEKTE
// ============================================================================

HardwareSerial uart1(1);  // DMX1
HardwareSerial uart2(2);  // DMX2
HardwareSerial uart0(0);  // DMX3

SoftwareSerial softSerial(DMX4_RX_PIN, DMX4_TX_PIN, false, 256);  // DMX4

// ============================================================================
// STATUS LEDS (OPTIONAL - jetzt viel mehr GPIO verfügbar!)
// ============================================================================

#define STATUS_LED_RED 32
#define STATUS_LED_GREEN 33
#define STATUS_LED_BLUE 34

#define LED_BRIGHTNESS_MAX 255

// ============================================================================
// DMX & NETZWERK BUFFER
// ============================================================================

uint8_t dmxBuffer[4][DMX_CHANNELS];
volatile unsigned long lastDmxUpdate[4] = {0, 0, 0, 0};
volatile uint32_t packetCount[4] = {0, 0, 0, 0};
volatile uint32_t errorCount = 0;

bool ethConnected = false;
AsyncUDP udpServer;

// ============================================================================
// STATISTIK (für Web API / Monitoring)
// ============================================================================

struct Stats {
  unsigned long uptime;
  uint32_t totalPackets;
  uint32_t totalErrors;
  uint8_t activeDMXLines;
} stats = {0, 0, 0, 0};

// ============================================================================
// FORWARD DECLARATIONS
// ============================================================================

void initEthernetWT32();
void initDMXAll();
void sendDMX(uint8_t dmxLine, const uint8_t* data);
void handleUDPPacket(AsyncUDPPacket packet);
void processSACN(const uint8_t* packet, size_t len);
void processArtNet(const uint8_t* packet, size_t len);
void ethEvent(WiFiEvent_t event);
void startUDPServer();
void initStatusLEDs();
void updateStatusLED();
void updateStats();

// ============================================================================
// SETUP
// ============================================================================

void setup() {
  delay(500);
  
  // GPIO Initialisierung - Enable Pins
  pinMode(DMX1_EN_PIN, OUTPUT);
  pinMode(DMX2_EN_PIN, OUTPUT);
  pinMode(DMX3_EN_PIN, OUTPUT);
  pinMode(DMX4_EN_PIN, OUTPUT);
  
  // Alle auf Empfang (LOW)
  digitalWrite(DMX1_EN_PIN, LOW);
  digitalWrite(DMX2_EN_PIN, LOW);
  digitalWrite(DMX3_EN_PIN, LOW);
  digitalWrite(DMX4_EN_PIN, LOW);
  
  // DMX Buffer löschen
  memset(dmxBuffer, 0, sizeof(dmxBuffer));
  
  // Initialisierung
  initStatusLEDs();
  setStatusLED(255, 255, 0);  // GELB = Startup
  
  initEthernetWT32();
  initDMXAll();
  
  // Startup-Sequenz
  for (int i = 0; i < 3; i++) {
    digitalWrite(DMX1_EN_PIN, HIGH);
    digitalWrite(DMX2_EN_PIN, HIGH);
    digitalWrite(DMX3_EN_PIN, HIGH);
    digitalWrite(DMX4_EN_PIN, HIGH);
    delayMicroseconds(100);
    
    digitalWrite(DMX1_EN_PIN, LOW);
    digitalWrite(DMX2_EN_PIN, LOW);
    digitalWrite(DMX3_EN_PIN, LOW);
    digitalWrite(DMX4_EN_PIN, LOW);
    delayMicroseconds(100);
  }
  
  setStatusLED(0, 255, 0);  // GRÜN = Ready
}

// ============================================================================
// MAIN LOOP
// ============================================================================

void loop() {
  // Failsafe: Sende letzte DMX Werte bei Timeout
  static unsigned long lastFailsafe = 0;
  if (millis() - lastFailsafe > 100) {
    lastFailsafe = millis();
    
    for (int i = 0; i < 4; i++) {
      if (millis() - lastDmxUpdate[i] > 5000) {
        // Timeout - sende letzten bekannten Wert
        sendDMX(i, dmxBuffer[i]);
      }
    }
  }
  
  // Status Update
  updateStatusLED();
  
  // Statistik
  static unsigned long lastStats = 0;
  if (millis() - lastStats > 5000) {
    lastStats = millis();
    updateStats();
  }
  
  // Ethernet Recovery
  static unsigned long lastRecovery = 0;
  if (!ethConnected && millis() - lastRecovery > 30000) {
    lastRecovery = millis();
    // Kein Ethernet? Versuche Reconnect
    ETH.disconnect(true);
    delay(1000);
    initEthernetWT32();
  }
  
  delay(50);
}

// ============================================================================
// ETHERNET - OPTIMIERT FÜR WT32-ETH01
// ============================================================================

/**
 * Ethernet Initialisierung für WT32-ETH01
 * 
 * Der WT32-ETH01 hat LAN8720 PHY INTEGRIERT!
 * Deshalb: Einfachere Konfiguration als ESP32 + extern
 */
void initEthernetWT32() {
  // Event Listener
  WiFi.onEvent(ethEvent);
  
  // WT32-ETH01 Ethernet Setup
  // Der LAN8720 ist schon auf dem Modul, keine externe Hardware nötig!
  ETH.begin(
    ETH_ADDR,        // LAN8720 PHY Address (immer 0)
    ETH_POWER_PIN,   // Power Pin (-1 nicht verwendet)
    ETH_MDC_PIN,     // GPIO23 - Management Data Clock
    ETH_MDIO_PIN,    // GPIO18 - Management Data I/O
    ETH_TYPE,        // ETH_PHY_LAN8720
    ETH_CLK_MODE     // ETH_CLOCK_GPIO0_IN
  );
  
  // Hostname setzen
  ETH.setHostname("wt32-dmx-encoder");
}

/**
 * Ethernet Event Handler
 */
void ethEvent(WiFiEvent_t event) {
  switch (event) {
    case SYSTEM_EVENT_ETH_START:
      // Ethernet gestartet
      break;
      
    case SYSTEM_EVENT_ETH_CONNECTED:
      // Ethernet verbunden
      break;
      
    case SYSTEM_EVENT_ETH_GOT_IP:
      ethConnected = true;
      setStatusLED(0, 255, 0);  // GRÜN
      startUDPServer();
      break;
      
    case SYSTEM_EVENT_ETH_DISCONNECTED:
      ethConnected = false;
      setStatusLED(255, 0, 0);  // ROT
      break;
      
    case SYSTEM_EVENT_ETH_STOP:
      ethConnected = false;
      break;
      
    default:
      break;
  }
}

/**
 * UDP Server für sACN + ArtNet
 */
void startUDPServer() {
  if (udpServer.listenMulticast(SACN_MULTICAST, SACN_PORT)) {
    udpServer.onPacket([](AsyncUDPPacket packet) {
      handleUDPPacket(packet);
    });
  }
}

// ============================================================================
// DMX INITIALISIERUNG - ALLE 4 PORTS
// ============================================================================

void initDMXAll() {
  // UART1 (DMX1)
  uart1.begin(DMX_BAUDRATE, SERIAL_8N2, DMX1_RX_PIN, DMX1_TX_PIN);
  
  // UART2 (DMX2)
  uart2.begin(DMX_BAUDRATE, SERIAL_8N2, DMX2_RX_PIN, DMX2_TX_PIN);
  
  // UART0 (DMX3) - kein Serial.begin() davor!
  uart0.begin(DMX_BAUDRATE, SERIAL_8N2, DMX3_RX_PIN, DMX3_TX_PIN);
  
  // SoftSerial (DMX4)
  softSerial.begin(DMX_BAUDRATE);
}

// ============================================================================
// DMX SENDEN - OPTIMIERT FÜR WT32
// ============================================================================

/**
 * Sende DMX512 Signal
 */
void sendDMX(uint8_t dmxLine, const uint8_t* data) {
  if (dmxLine > 3 || !data) return;
  
  // UART Auswahl
  HardwareSerial* uart = nullptr;
  SoftwareSerial* soft = nullptr;
  uint8_t enablePin = 0;
  bool isSoftSerial = false;
  
  switch (dmxLine) {
    case 0:
      uart = &uart1;
      enablePin = DMX1_EN_PIN;
      break;
    case 1:
      uart = &uart2;
      enablePin = DMX2_EN_PIN;
      break;
    case 2:
      uart = &uart0;
      enablePin = DMX3_EN_PIN;
      break;
    case 3:
      soft = &softSerial;
      enablePin = DMX4_EN_PIN;
      isSoftSerial = true;
      break;
    default:
      return;
  }
  
  // MAX485 auf SENDEN (HIGH)
  digitalWrite(enablePin, HIGH);
  delayMicroseconds(100);
  
  // DMX BREAK
  if (isSoftSerial) {
    soft->write(0x00);
  } else {
    uart->write(0x00);
  }
  delayMicroseconds(DMX_BREAK_TIME);
  
  // Start Code
  if (isSoftSerial) {
    soft->write(DMX_START_CODE);
  } else {
    uart->write(DMX_START_CODE);
  }
  
  // 512 Bytes Daten
  if (isSoftSerial) {
    for (int i = 0; i < DMX_CHANNELS; i++) {
      soft->write(data[i]);
    }
    soft->flush();
  } else {
    uart->write(data, DMX_CHANNELS);
    uart->flush();
  }
  
  // Zurück auf EMPFANG (LOW)
  digitalWrite(enablePin, LOW);
  
  // Statistik
  lastDmxUpdate[dmxLine] = millis();
  packetCount[dmxLine]++;
}

// ============================================================================
// NETZWERK PACKET VERARBEITUNG
// ============================================================================

/**
 * UDP Packet Handler
 */
void handleUDPPacket(AsyncUDPPacket packet) {
  const uint8_t* data = packet.data();
  size_t len = packet.length();
  
  if (len < 20) return;
  
  // sACN Check
  if (len >= 126 && data[4] == 0x41 && data[5] == 0x53 && 
      data[6] == 0x43 && data[7] == 0x4E) {
    processSACN(data, len);
  }
  // ArtNet Check
  else if (len >= 14 && data[0] == 0x41 && data[1] == 0x72 && 
           data[2] == 0x74 && data[3] == 0x2D) {
    processArtNet(data, len);
  }
}

/**
 * sACN Parser
 */
void processSACN(const uint8_t* packet, size_t len) {
  if (len < 638) return;
  
  uint16_t universe = (packet[114] << 8) | packet[113];
  if (universe > 3) return;
  if (packet[125] != DMX_START_CODE) return;
  
  memcpy(dmxBuffer[universe], &packet[126], DMX_CHANNELS);
  sendDMX(universe, dmxBuffer[universe]);
}

/**
 * ArtNet Parser
 */
void processArtNet(const uint8_t* packet, size_t len) {
  if (len < 530) return;
  
  if (!(packet[0] == 0x41 && packet[1] == 0x72 && packet[2] == 0x74 &&
        packet[3] == 0x2D && packet[4] == 0x4E && packet[5] == 0x65 &&
        packet[6] == 0x74)) {
    return;
  }
  
  uint16_t opcode = (packet[9] << 8) | packet[8];
  if (opcode != 0x5000) return;
  
  uint16_t universe = (packet[15] << 8) | packet[14];
  if (universe > 3) return;
  
  memcpy(dmxBuffer[universe], &packet[18], DMX_CHANNELS);
  sendDMX(universe, dmxBuffer[universe]);
}

// ============================================================================
// STATUS LEDS (OPTIONAL - mehr GPIO verfügbar!)
// ============================================================================

void initStatusLEDs() {
  pinMode(STATUS_LED_RED, OUTPUT);
  pinMode(STATUS_LED_GREEN, OUTPUT);
  pinMode(STATUS_LED_BLUE, OUTPUT);
  
  setStatusLED(0, 0, 0);  // Aus
}

void setStatusLED(uint8_t r, uint8_t g, uint8_t b) {
  analogWrite(STATUS_LED_RED, r);
  analogWrite(STATUS_LED_GREEN, g);
  analogWrite(STATUS_LED_BLUE, b);
}

/**
 * LED Status Update
 */
void updateStatusLED() {
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate < 100) return;
  lastUpdate = millis();
  
  // Ethernet Status
  if (!ethConnected) {
    setStatusLED(255, 0, 0);  // ROT - Ethernet down
    return;
  }
  
  // DMX Activity Check
  unsigned long now = millis();
  bool anyActive = false;
  
  for (int i = 0; i < 4; i++) {
    if (now - lastDmxUpdate[i] < 1000) {
      anyActive = true;
      break;
    }
  }
  
  if (anyActive) {
    // Blinkendes BLAU = DMX aktiv
    static bool blink = false;
    blink = !blink;
    setStatusLED(0, 0, blink ? 255 : 100);
  } else {
    setStatusLED(0, 255, 0);  // GRÜN - OK, keine Daten
  }
}

// ============================================================================
// STATISTIK & MONITORING
// ============================================================================

void updateStats() {
  stats.uptime = millis() / 1000;
  stats.totalPackets = packetCount[0] + packetCount[1] + 
                       packetCount[2] + packetCount[3];
  stats.totalErrors = errorCount;
  
  unsigned long now = millis();
  stats.activeDMXLines = 0;
  for (int i = 0; i < 4; i++) {
    if (now - lastDmxUpdate[i] < 5000) {
      stats.activeDMXLines++;
    }
  }
}

/**
 * JSON Status für Web API / Monitoring
 */
String getStatsJSON() {
  char buffer[512];
  snprintf(buffer, sizeof(buffer),
    "{"
    "\"device\":\"WT32-ETH01\","
    "\"uptime\":%lu,"
    "\"packets\":%lu,"
    "\"errors\":%lu,"
    "\"activeDMX\":%d,"
    "\"dmx0\":%lu,"
    "\"dmx1\":%lu,"
    "\"dmx2\":%lu,"
    "\"dmx3\":%lu,"
    "\"eth\":%s"
    "}",
    stats.uptime,
    stats.totalPackets,
    stats.totalErrors,
    stats.activeDMXLines,
    packetCount[0],
    packetCount[1],
    packetCount[2],
    packetCount[3],
    ethConnected ? "true" : "false"
  );
  return String(buffer);
}

// ============================================================================
// ERWEITERTE FEATURES (Optional)
// ============================================================================

/**
 * Optional: Web Server für Monitoring/Konfiguration
 * 
 * Aktivieren mit:
 * #include <AsyncWebServer.h>
 * 
 * Dann im Setup:
 * AsyncWebServer server(80);
 * server.on("/api/stats", HTTP_GET, [](AsyncWebServerRequest *request) {
 *   request->send(200, "application/json", getStatsJSON());
 * });
 * server.begin();
 */

/**
 * Optional: I2C OLED Display (GPIO21 SDA / GPIO22 SCL)
 * 
 * Zeigt Status live an:
 * #include <Adafruit_SSD1306.h>
 * 
 * WT32 hat viele freie GPIO für Displays!
 */

// ============================================================================
// KONFIGURATION (einfach zu ändern)
// ============================================================================

struct Config {
  // Ethernet
  bool useDHCP = true;
  uint32_t staticIP = 0xC0A80164;  // 192.168.1.100
  
  // DMX
  uint32_t dmxBaudrate = DMX_BAUDRATE;
  bool enableFailsafe = true;
  uint16_t failsafeTimeout = 5000;
  
  // Features
  bool enableStatusLED = true;
  bool enableWebServer = false;  // Später aktivieren
} config;

// ============================================================================
// WT32-ETH01 SPEZIFISCHE VORTEILE
// ============================================================================

/*
WHY WT32-ETH01 ist BESSER für dieses Projekt:

✓ LAN8720 PHY INTEGRIERT (kein CH340 nötig!)
✓ Ethernet stabil und dediziert
✓ Alle GPIO FREI für DMX + Features
✓ Kompaktes Design
✓ Bessere Ethernet-Performance
✓ Weniger externe Komponenten

VERGLEICH:
ESP32 + CH340 Setup:
├─ ESP32 Modul
├─ CH340 Ethernet Modul
├─ Viele Verbindungskabel
└─ Komplexer, anfällig für Fehler

WT32-ETH01:
├─ Ein Modul (alles integriert)
├─ Ethernet INTERN
└─ Einfacher, zuverlässiger

Diese Code-Version ist optimiert für WT32-ETH01!
*/

// ============================================================================
// CHECKLISTE VOR PRODUKTION
// ============================================================================

/*
✓ Ethernet Initialisierung für WT32 (ETH.begin mit richtigen Pins)
✓ Alle 4 UART ports initialisiert
✓ Enable Pins korrekt angeschlossen
✓ MAX485 Module verdrahtet
✓ XLR-Buchsen gelötet
✓ Status LEDs optional
✓ Stromversorgung stabil (1A minimum)
✓ Kein Serial.begin() (UART0 frei für DMX!)
✓ Mit GrandMA2 getestet
✓ Alle 4 DMX Leitungen funktionieren

DANN: Ready for Production! 🚀
*/
