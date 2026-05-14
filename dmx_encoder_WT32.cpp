/**
 * WT32-ETH01 - DMX Encoder (4x DMX) - KORRIGIERT FÜR ESP32 3.3.8+
 * ✨ MIT INTEGRIERTEM RS485 PORT FÜR DMX4
 * ✅ NEUE ETH API (ESP32 Arduino Core 3.3.8+)
 */

#include <Arduino.h>
#include <WiFi.h>
#include <ETH.h>
#include <AsyncUDP.h>
#include <HardwareSerial.h>

// ============================================================================
// WT32-ETH01 ETHERNET KONFIGURATION (neue API)
// ============================================================================

#define ETH_PHY_ADDR 0
#define ETH_POWER_PIN -1
#define ETH_MDC_PIN 23
#define ETH_MDIO_PIN 18
#define ETH_PHY_TYPE ETH_PHY_LAN8720
#define ETH_CLK_MODE ETH_CLOCK_GPIO0_IN

// ============================================================================
// DMX HARDWARE - 4 HARDWARE UARTs
// ============================================================================

// DMX1: UART1 (TX=9, RX=10) + MAX485 @ IO27
#define DMX1_TX_PIN 9
#define DMX1_RX_PIN 10
#define DMX1_EN_PIN 27

// DMX2: UART0 (TX=1, RX=3) + MAX485 @ IO25
#define DMX2_TX_PIN 1
#define DMX2_RX_PIN 3
#define DMX2_EN_PIN 25

// DMX3: UART1 auf GPIO16/17 + MAX485 @ IO26
#define DMX3_TX_PIN 16
#define DMX3_RX_PIN 17
#define DMX3_EN_PIN 26

// DMX4: UART2 INTEGRIERT RS485 (TX=IO17, RX=IO5) + 485_EN @ IO33
#define DMX4_TX_PIN 17
#define DMX4_RX_PIN 5
#define DMX4_EN_PIN 33

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
HardwareSerial uart2(2);  // DMX2 (UART0)
HardwareSerial uart3(3);  // DMX3 (UART1)
HardwareSerial uart4(2);  // DMX4 (UART2 - integrierter RS485)

// ============================================================================
// STATUS LEDS - IO33 ist RS485_EN, nicht für LED!
// ============================================================================

#define STATUS_LED_RED 36
#define STATUS_LED_GREEN 39
#define STATUS_LED_BLUE 34

// ============================================================================
// BUFFER & STATISTIK
// ============================================================================

uint8_t dmxBuffer[4][DMX_CHANNELS];
volatile unsigned long lastDmxUpdate[4] = {0, 0, 0, 0};
volatile uint32_t packetCount[4] = {0, 0, 0, 0};
volatile uint32_t errorCount = 0;

bool ethConnected = false;
AsyncUDP udpServer;

// ============================================================================
// FORWARD DECLARATIONS
// ============================================================================

void initEthernetWT32();
void initDMXAll();
void sendDMX(uint8_t dmxLine, const uint8_t* data);
void handleUDPPacket(AsyncUDPPacket packet);
void processSACN(const uint8_t* packet, size_t len);
void processArtNet(const uint8_t* packet, size_t len);
void ethEvent(arduino_event_id_t event);
void startUDPServer();
void initStatusLEDs();
void setStatusLED(uint8_t r, uint8_t g, uint8_t b);
void updateStatusLED();

// ============================================================================
// SETUP
// ============================================================================

void setup() {
  delay(500);
  
  // Serial für Debug (UART0 alternative)
  Serial.begin(115200);
  delay(100);
  Serial.println("\n\n=== WT32-ETH01 DMX Encoder Start ===");
  
  // GPIO Initialisierung - Enable Pins für MAX485 Modules
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
  
  Serial.println("✓ Setup complete!");
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
        sendDMX(i, dmxBuffer[i]);
      }
    }
  }
  
  // Status Update
  updateStatusLED();
  
  // Ethernet Recovery (neue API: kein disconnect nötig)
  static unsigned long lastRecovery = 0;
  if (!ethConnected && millis() - lastRecovery > 30000) {
    lastRecovery = millis();
    Serial.println("⚠️ Ethernet Reconnect...");
    // In der neuen API ist ETH.disconnect() nicht nötig
    // Das System versucht automatisch zu reconnecten
  }
  
  delay(50);
}

// ============================================================================
// ETHERNET - OPTIMIERT FÜR WT32-ETH01 (neue API 3.3.8+)
// ============================================================================

void initEthernetWT32() {
  // Event Listener (neue API!)
  WiFi.onEvent(ethEvent);
  
  Serial.println("Initializing Ethernet...");
  
  // WT32-ETH01 Ethernet Setup (NEUE API)
  // ETH.begin(phy_type, phy_addr, mdc, mdio, power, clk_mode)
  bool eth_ok = ETH.begin(
    ETH_PHY_TYPE,      // ETH_PHY_LAN8720 (oder ETH_PHY_TYPE_LAN8720)
    ETH_PHY_ADDR,      // PHY Address (0)
    ETH_MDC_PIN,       // GPIO23 - Management Data Clock
    ETH_MDIO_PIN,      // GPIO18 - Management Data I/O
    ETH_POWER_PIN,     // Power Pin (-1 nicht verwendet)
    ETH_CLK_MODE       // ETH_CLOCK_GPIO0_IN
  );
  
  if (eth_ok) {
    Serial.println("✓ Ethernet initialized");
    ETH.setHostname("wt32-dmx-encoder");
  } else {
    Serial.println("✗ Ethernet init failed!");
  }
}

/**
 * Ethernet Event Handler (NEUE API!)
 * arduino_event_id_t statt WiFiEvent_t
 * IP_EVENT_ETH_* statt SYSTEM_EVENT_ETH_*
 */
void ethEvent(arduino_event_id_t event) {
  switch (event) {
    case ARDUINO_EVENT_ETH_START:
      Serial.println("Ethernet started");
      break;
      
    case ARDUINO_EVENT_ETH_CONNECTED:
      Serial.println("Ethernet connected");
      break;
      
    case ARDUINO_EVENT_ETH_GOT_IP:
      Serial.println("✓ Ethernet: Got IP!");
      ethConnected = true;
      setStatusLED(0, 255, 0);  // GRÜN
      startUDPServer();
      break;
      
    case ARDUINO_EVENT_ETH_DISCONNECTED:
      Serial.println("⚠️ Ethernet disconnected");
      ethConnected = false;
      setStatusLED(255, 0, 0);  // ROT
      break;
      
    case ARDUINO_EVENT_ETH_STOP:
      Serial.println("Ethernet stopped");
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
  Serial.println("Starting UDP server...");
  if (udpServer.listenMulticast(SACN_MULTICAST, SACN_PORT)) {
    Serial.println("✓ UDP Server listening on 239.69.255.255:5568");
    udpServer.onPacket([](AsyncUDPPacket packet) {
      handleUDPPacket(packet);
    });
  } else {
    Serial.println("✗ UDP Server failed!");
  }
}

// ============================================================================
// DMX INITIALISIERUNG - 4x UART
// ============================================================================

void initDMXAll() {
  Serial.println("\nInitializing DMX...");
  
  // DMX1: UART1 (Standard Pins)
  uart1.begin(DMX_BAUDRATE, SERIAL_8N2, DMX1_RX_PIN, DMX1_TX_PIN);
  uart1.setRxBufferSize(2048);
  Serial.println("✓ DMX1: UART1 (TX=9, RX=10)");
  
  // DMX2: UART0 (Serial0, aber mit custom pins)
  uart2.begin(DMX_BAUDRATE, SERIAL_8N2, DMX2_RX_PIN, DMX2_TX_PIN);
  uart2.setRxBufferSize(2048);
  Serial.println("✓ DMX2: UART0 (TX=1, RX=3)");
  
  // DMX3: UART1 auf unterschiedliche Pins
  uart3.begin(DMX_BAUDRATE, SERIAL_8N2, DMX3_RX_PIN, DMX3_TX_PIN);
  uart3.setRxBufferSize(2048);
  Serial.println("✓ DMX3: UART1 alt (TX=16, RX=17)");
  
  // DMX4: UART2 - INTEGRIERTER RS485 PORT
  uart4.begin(DMX_BAUDRATE, SERIAL_8N2, DMX4_RX_PIN, DMX4_TX_PIN);
  uart4.setRxBufferSize(2048);
  Serial.println("✓ DMX4: UART2 RS485 (TX=17, RX=5, EN=33) - INTEGRIERT!");
}

// ============================================================================
// DMX SENDEN
// ============================================================================

void sendDMX(uint8_t dmxLine, const uint8_t* data) {
  if (dmxLine > 3) return;
  
  HardwareSerial* uart = nullptr;
  uint8_t enablePin = 0;
  
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
      uart = &uart3;
      enablePin = DMX3_EN_PIN;
      break;
    case 3:
      uart = &uart4;  // Integrierter RS485
      enablePin = DMX4_EN_PIN;
      break;
    default:
      return;
  }
  
  // MAX485 / RS485 auf SENDEN (HIGH)
  digitalWrite(enablePin, HIGH);
  delayMicroseconds(100);
  
  // DMX BREAK
  uart->write(0x00);
  delayMicroseconds(DMX_BREAK_TIME);
  
  // Start Code
  uart->write(DMX_START_CODE);
  
  // 512 Bytes Daten
  uart->write(data, DMX_CHANNELS);
  uart->flush();
  
  // Zurück auf EMPFANG (LOW)
  digitalWrite(enablePin, LOW);
  
  // Statistik
  lastDmxUpdate[dmxLine] = millis();
  packetCount[dmxLine]++;
}

// ============================================================================
// NETZWERK PACKET VERARBEITUNG
// ============================================================================

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

void processSACN(const uint8_t* packet, size_t len) {
  if (len < 638) return;
  
  uint16_t universe = (packet[114] << 8) | packet[113];
  if (universe > 3) return;
  if (packet[125] != DMX_START_CODE) return;
  
  memcpy(dmxBuffer[universe], &packet[126], DMX_CHANNELS);
  sendDMX(universe, dmxBuffer[universe]);
}

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
// STATUS LEDS
// ============================================================================

void initStatusLEDs() {
  // ⚠️ IO33 ist 485_EN - nicht für LED!
  // Verwende IO36, IO39, IO34
  
  pinMode(STATUS_LED_RED, OUTPUT);
  pinMode(STATUS_LED_GREEN, OUTPUT);
  pinMode(STATUS_LED_BLUE, OUTPUT);
  
  setStatusLED(0, 0, 0);
}

void setStatusLED(uint8_t r, uint8_t g, uint8_t b) {
  analogWrite(STATUS_LED_RED, r);
  analogWrite(STATUS_LED_GREEN, g);
  analogWrite(STATUS_LED_BLUE, b);
}

void updateStatusLED() {
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate < 100) return;
  lastUpdate = millis();
  
  if (!ethConnected) {
    setStatusLED(255, 0, 0);
    return;
  }
  
  unsigned long now = millis();
  bool anyActive = false;
  
  for (int i = 0; i < 4; i++) {
    if (now - lastDmxUpdate[i] < 1000) {
      anyActive = true;
      break;
    }
  }
  
  if (anyActive) {
    static bool blink = false;
    blink = !blink;
    setStatusLED(0, 0, blink ? 255 : 100);
  } else {
    setStatusLED(0, 255, 0);
  }
}

// ============================================================================
// ÄNDERUNGEN für ESP32 Arduino Core 3.3.8+
// ============================================================================

/*
WICHTIGE ÄNDERUNGEN in der neuen API:

1. ETH Events:
   ALT: SYSTEM_EVENT_ETH_GOT_IP
   NEU: ARDUINO_EVENT_ETH_GOT_IP oder IP_EVENT_ETH_GOT_IP
   
2. ETH.disconnect() existiert nicht mehr
   - Das System reconnectet automatisch
   
3. ETH.begin() Parameterreihenfolge:
   ALT: ETH.begin(addr, power, mdc, mdio, type, clk)
   NEU: ETH.begin(type, addr, mdc, mdio, power, clk)
   
4. PHY Type Konstanten:
   ALT: ETH_PHY_LAN8720
   NEU: ETH_PHY_LAN8720 oder ETH_PHY_TYPE_LAN8720
   
Alle diese Änderungen sind in diesem Code berücksichtigt!
*/
