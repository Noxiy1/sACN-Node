/**
 * ESP32 ETH01 - DMX Encoder
 * sACN (E1.31) und ArtNet zu 4x DMX512 Konverter
 * 
 * Hardware-Setup:
 * - ESP32 ETH01 mit Ethernet
 * - 4x MAX485 Module
 * - 4x XLR-3 Buchsen (DMX Out)
 * 
 * Autor: Auto-generated
 * Version: 1.0
 */

#include <Arduino.h>
#include <WiFi.h>
#include <ETH.h>
#include <AsyncUDP.h>
#include <HardwareSerial.h>

// ============================================================================
// KONFIGURATION
// ============================================================================

// Ethernet-Konfiguration (für ETH01)
#define ETH_PHY_ADDR 0
#define ETH_PHY_POWER -1
#define ETH_PHY_MDC 23
#define ETH_PHY_MDIO 18
#define ETH_CLK_MODE ETH_CLOCK_GPIO0_IN

// UART Pins für MAX485 Module (DMX Ausgänge)
// DMX 1: UART1 (GPIO9, GPIO10)
// DMX 2: UART2 (GPIO16, GPIO17)
// DMX 3: UART0 (GPIO1, GPIO3) - Achtung: Wird auch für USB verwendet
// DMX 4: Softserial (GPIO4, GPIO5)

#define DMX_UART1_TX 9
#define DMX_UART1_RX 10
#define DMX_UART2_TX 16
#define DMX_UART2_RX 17

// MAX485 Control Pins (DE/RE für Senden/Empfangen)
#define MAX485_1_EN 27
#define MAX485_2_EN 26
#define MAX485_3_EN 25
#define MAX485_4_EN 14

// DMX-Konstanten
#define DMX_BREAK_TIME 200      // Break in µs
#define DMX_MAB_TIME 80         // Mark After Break in µs
#define DMX_CHANNELS 512        // Standard DMX512
#define DMX_BAUDRATE 250000     // DMX Baudrate

// sACN/ArtNet Ports
#define SACN_PORT 5568
#define ARTNET_PORT 6454

// ============================================================================
// GLOBALE VARIABLEN
// ============================================================================

// DMX-Daten Buffer (4 Universen à 512 Kanäle)
uint8_t dmxBuffer[4][DMX_CHANNELS];
volatile unsigned long lastDmxUpdate[4] = {0, 0, 0, 0};

// Ethernet Status
bool ethConnected = false;

// UDP Server
AsyncUDP udpServer;

// UART Objekte
HardwareSerial uart1(1);
HardwareSerial uart2(2);

// ============================================================================
// FORWARD DECLARATIONS
// ============================================================================

void initEthernet();
void initDMX();
void sendDMX(uint8_t dmxLine, const uint8_t* data);
void processSACN(const uint8_t* packet, size_t len);
void processArtNet(const uint8_t* packet, size_t len);
void handleUDPPacket(AsyncUDPPacket packet);
void ethEvent(WiFiEvent_t event);

// ============================================================================
// SETUP
// ============================================================================

void setup() {
  // Serial für Debug (USB)
  Serial.begin(115200);
  delay(100);
  
  Serial.println("\n\n================================================");
  Serial.println("  ESP32 DMX Encoder - Boot Start");
  Serial.println("  sACN & ArtNet to 4x DMX512");
  Serial.println("================================================\n");
  
  // GPIO Initialisierung
  pinMode(MAX485_1_EN, OUTPUT);
  pinMode(MAX485_2_EN, OUTPUT);
  pinMode(MAX485_3_EN, OUTPUT);
  pinMode(MAX485_4_EN, OUTPUT);
  
  // Alle MAX485 auf Empfang (LOW = Empfang)
  digitalWrite(MAX485_1_EN, LOW);
  digitalWrite(MAX485_2_EN, LOW);
  digitalWrite(MAX485_3_EN, LOW);
  digitalWrite(MAX485_4_EN, LOW);
  
  // DMX Buffer initialisieren
  memset(dmxBuffer, 0, sizeof(dmxBuffer));
  
  // Hardware initialisieren
  initEthernet();
  initDMX();
  
  Serial.println("✓ Setup abgeschlossen\n");
}

// ============================================================================
// LOOP
// ============================================================================

void loop() {
  // Status Check alle 10 Sekunden
  static unsigned long lastStatus = 0;
  if (millis() - lastStatus > 10000) {
    lastStatus = millis();
    
    Serial.println("\n--- STATUS ---");
    Serial.printf("Ethernet: %s\n", ethConnected ? "✓ Verbunden" : "✗ Getrennt");
    
    for (int i = 0; i < 4; i++) {
      unsigned long lastUpdate = millis() - lastDmxUpdate[i];
      Serial.printf("DMX %d: %s (%lums)\n", 
        i + 1,
        lastUpdate < 5000 ? "✓ Aktiv" : "✗ Inaktiv",
        lastUpdate
      );
    }
    Serial.println();
  }
  
  delay(100);
}

// ============================================================================
// ETHERNET INITIALISIERUNG
// ============================================================================

void initEthernet() {
  Serial.println("Initialisiere Ethernet...");
  
  // Event Listener registrieren
  WiFi.onEvent(ethEvent);
  
  // Ethernet mit Pin-Konfiguration starten
  ETH.begin(
    ETH_PHY_ADDR,
    ETH_PHY_POWER,
    ETH_PHY_MDC,
    ETH_PHY_MDIO,
    ETH_PHY_TYPE,
    ETH_CLK_MODE
  );
  
  Serial.println("Warte auf Ethernet-Verbindung...");
}

/**
 * Ethernet Event Handler
 */
void ethEvent(WiFiEvent_t event) {
  switch (event) {
    case SYSTEM_EVENT_ETH_START:
      Serial.println("✓ Ethernet gestartet");
      ETH.setHostname("DMX-Encoder");
      break;
      
    case SYSTEM_EVENT_ETH_CONNECTED:
      Serial.println("✓ Ethernet verbunden");
      break;
      
    case SYSTEM_EVENT_ETH_GOT_IP:
      ethConnected = true;
      Serial.printf("✓ IP-Adresse: %s\n", ETH.localIP().toString().c_str());
      Serial.printf("✓ Netmask: %s\n", ETH.subnetMask().toString().c_str());
      Serial.printf("✓ Gateway: %s\n", ETH.gatewayIP().toString().c_str());
      
      // UDP Server starten
      startUDPServer();
      break;
      
    case SYSTEM_EVENT_ETH_DISCONNECTED:
      ethConnected = false;
      Serial.println("✗ Ethernet getrennt");
      break;
      
    case SYSTEM_EVENT_ETH_STOP:
      Serial.println("✗ Ethernet gestoppt");
      ethConnected = false;
      break;
      
    default:
      break;
  }
}

/**
 * UDP Server für sACN und ArtNet starten
 */
void startUDPServer() {
  // sACN (Port 5568)
  if (udpServer.listenMulticast(IPAddress(239, 69, 255, 255), SACN_PORT)) {
    Serial.printf("✓ sACN Server läuft auf Port %d\n", SACN_PORT);
    
    udpServer.onPacket([](AsyncUDPPacket packet) {
      handleUDPPacket(packet);
    });
  } else {
    Serial.println("✗ Fehler beim Start des UDP Servers");
  }
}

// ============================================================================
// DMX INITIALISIERUNG
// ============================================================================

void initDMX() {
  Serial.println("Initialisiere DMX UARTs...");
  
  // UART1 für DMX 1
  uart1.begin(DMX_BAUDRATE, SERIAL_8N2, DMX_UART1_RX, DMX_UART1_TX);
  Serial.println("✓ DMX 1 (UART1) initialisiert");
  
  // UART2 für DMX 2
  uart2.begin(DMX_BAUDRATE, SERIAL_8N2, DMX_UART2_RX, DMX_UART2_TX);
  Serial.println("✓ DMX 2 (UART2) initialisiert");
  
  // DMX 3 & 4: Softserial über bit banging würde hier implementiert
  // Für dieses Projekt: Nur 2 Hardware UARTs genutzt, oder
  // Externe UART-Chips (z.B. CH340) über I2C
  
  Serial.println("✓ DMX Ausgänge bereit\n");
}

// ============================================================================
// DMX SENDEN
// ============================================================================

/**
 * DMX-Signal auf UART senden
 * Format: BREAK (88µs) -> MAB (8µs) -> Start Code (0x00) -> 512 Bytes Daten
 * 
 * @param dmxLine: 0-3 (DMX Ausgang)
 * @param data: Pointer auf 512-Byte Array
 */
void sendDMX(uint8_t dmxLine, const uint8_t* data) {
  if (dmxLine > 3) return;
  
  HardwareSerial* uart = nullptr;
  uint8_t enablePin = 0;
  
  // Richtige UART und Enable-Pin wählen
  switch (dmxLine) {
    case 0:
      uart = &uart1;
      enablePin = MAX485_1_EN;
      break;
    case 1:
      uart = &uart2;
      enablePin = MAX485_2_EN;
      break;
    case 2:
      enablePin = MAX485_3_EN;
      // DMX 3: Hardware implementieren
      return;
    case 3:
      enablePin = MAX485_4_EN;
      // DMX 4: Hardware implementieren
      return;
  }
  
  if (!uart) return;
  
  // MAX485 auf SENDEN umschalten (HIGH = Senden)
  digitalWrite(enablePin, HIGH);
  delayMicroseconds(50);
  
  // DMX BREAK: Mindestens 88µs (wir machen 200µs für Sicherheit)
  uart->write(0x00);  // Dummy-Byte für Break-Timing
  delayMicroseconds(DMX_BREAK_TIME);
  
  // Start Code (0x00)
  uart->write(0x00);
  
  // 512 Bytes DMX-Daten senden
  uart->write(data, DMX_CHANNELS);
  
  // Warten, bis alles gesendet ist
  uart->flush();
  
  // Zurück auf EMPFANG
  digitalWrite(enablePin, LOW);
  
  // Update Timestamp
  lastDmxUpdate[dmxLine] = millis();
}

// ============================================================================
// NETZWERK PACKET VERARBEITUNG
// ============================================================================

/**
 * UDP Packet Handler - Erkenne sACN oder ArtNet
 */
void handleUDPPacket(AsyncUDPPacket packet) {
  const uint8_t* data = packet.data();
  size_t len = packet.length();
  
  // Minimum Packet Größe Check
  if (len < 20) return;
  
  // sACN Identifikation: "ACN-E131" Preamble
  if (len >= 126 && data[4] == 0x41 && data[5] == 0x53 && 
      data[6] == 0x43 && data[7] == 0x4E) {  // "ASN" (sACN)
    processSACN(data, len);
  }
  // ArtNet Identifikation: "Art-Net\0" (0x4172742D4E65743D)
  else if (len >= 14 && data[0] == 0x41 && data[1] == 0x72 && 
           data[2] == 0x74 && data[3] == 0x2D) {  // "Art-"
    processArtNet(data, len);
  }
}

/**
 * sACN (E1.31) Packet verarbeiten
 * 
 * Struktur:
 * - Preamble: 16 Bytes (0x0000, Length)
 * - ACN Header: "ACN-E131"
 * - DMP Header: DMX Universe und Daten
 */
void processSACN(const uint8_t* packet, size_t len) {
  // Prüfe Minimum Größe: Preamble(16) + ACN(38) + Framing(77) + DMP(11) + Daten(513)
  if (len < 126) return;
  
  // Universe extrahieren (Bytes 113-114, Little Endian)
  uint16_t universe = (packet[114] << 8) | packet[113];
  
  // Nur Universe 0-3 verarbeiten (für 4 DMX Leitungen)
  if (universe > 3) return;
  
  // DMX Start Code prüfen (sollte 0x00 sein)
  if (packet[125] != 0x00) return;
  
  // 512 Bytes DMX-Daten kopieren (ab Byte 126)
  if (len >= 638) {  // 126 + 512
    memcpy(dmxBuffer[universe], &packet[126], DMX_CHANNELS);
    
    // Sende auf entsprechender DMX-Leitung
    sendDMX(universe, dmxBuffer[universe]);
    
    // Status-Update
    static unsigned long lastLog = 0;
    if (millis() - lastLog > 1000) {
      lastLog = millis();
      Serial.printf("sACN Universe %d -> DMX %d ✓\n", universe, universe + 1);
    }
  }
}

/**
 * ArtNet Packet verarbeiten
 * 
 * Struktur:
 * - ID: "Art-Net\0" (Bytes 0-7)
 * - OpCode: (Bytes 8-9, Little Endian)
 * - ProtoVer: (Bytes 10-11)
 * - Sequence: (Byte 12)
 * - Physical: (Byte 13)
 * - Universe: (Bytes 14-15, Little Endian)
 * - DMX: (Bytes 18-529, 512 Bytes)
 */
void processArtNet(const uint8_t* packet, size_t len) {
  // Prüfe Minimum Größe
  if (len < 530) return;
  
  // ID Check: "Art-Net\0"
  if (!(packet[0] == 0x41 && packet[1] == 0x72 && packet[2] == 0x74 &&
        packet[3] == 0x2D && packet[4] == 0x4E && packet[5] == 0x65 &&
        packet[6] == 0x74)) {
    return;
  }
  
  // OpCode Check: 0x5000 (ArtDMX)
  uint16_t opcode = (packet[9] << 8) | packet[8];
  if (opcode != 0x5000) return;
  
  // Universe extrahieren (Bytes 14-15, Little Endian)
  uint16_t universe = (packet[15] << 8) | packet[14];
  
  // Nur Universe 0-3
  if (universe > 3) return;
  
  // 512 Bytes DMX-Daten kopieren (ab Byte 18)
  memcpy(dmxBuffer[universe], &packet[18], DMX_CHANNELS);
  
  // Sende auf entsprechender DMX-Leitung
  sendDMX(universe, dmxBuffer[universe]);
  
  // Status
  static unsigned long lastLog = 0;
  if (millis() - lastLog > 1000) {
    lastLog = millis();
    Serial.printf("ArtNet Universe %d -> DMX %d ✓\n", universe, universe + 1);
  }
}

// ============================================================================
// UTILITY FUNKTIONEN
// ============================================================================

/**
 * Debug: Zeige DMX Werte für ein Universe
 */
void printDMXValues(uint8_t universe, uint8_t startChannel, uint8_t count) {
  if (universe > 3) return;
  
  Serial.printf("DMX %d, Kanäle %d-%d: ", 
    universe + 1, startChannel, startChannel + count - 1);
  
  for (int i = 0; i < count; i++) {
    Serial.printf("%3d ", dmxBuffer[universe][startChannel + i - 1]);
  }
  Serial.println();
}

/**
 * Wiederaufbau nach Fehler
 */
void recovery() {
  Serial.println("\n⚠ Recovery gestartet...");
  
  // Reset Ethernet
  if (!ethConnected) {
    Serial.println("Versuche Ethernet-Reconnect...");
    ETH.disconnect();
    delay(1000);
    ETH.begin(ETH_PHY_ADDR, ETH_PHY_POWER, ETH_PHY_MDC, ETH_PHY_MDIO,
              ETH_PHY_TYPE, ETH_CLK_MODE);
  }
  
  // Alle DMX Buffer löschen
  memset(dmxBuffer, 0, sizeof(dmxBuffer));
  
  Serial.println("✓ Recovery abgeschlossen\n");
}
