/**
 * config.h
 * 
 * Zentrale Konfigurationsdatei
 * Alle wichtigen Parameter hier - keine Code-Änderungen nötig!
 */

#pragma once

#include <Arduino.h>

// ============================================================================
// ETHERNET & NETZWERK KONFIGURATION
// ============================================================================

// Ethernet Interfaces (für ESP32-ETH01)
#define ETH_PHY_ADDR 0
#define ETH_PHY_POWER -1
#define ETH_PHY_MDC 23
#define ETH_PHY_MDIO 18
#define ETH_PHY_TYPE ETH_PHY_LAN8720
#define ETH_CLK_MODE ETH_CLOCK_GPIO0_IN

// IP-Konfiguration
// true = DHCP, false = Statisch
#define USE_DHCP true

// Statische IP (nur wenn USE_DHCP = false)
#define STATIC_IP IPAddress(192, 168, 1, 100)
#define STATIC_NETMASK IPAddress(255, 255, 255, 0)
#define STATIC_GATEWAY IPAddress(192, 168, 1, 1)
#define STATIC_DNS IPAddress(8, 8, 8, 8)

// Hostname
#define HOSTNAME "dmx-encoder"

// ============================================================================
// NETZWERK PORTS
// ============================================================================

#define SACN_PORT 5568
#define ARTNET_PORT 6454

// sACN Multicast
#define SACN_MULTICAST IPAddress(239, 69, 255, 255)

// ============================================================================
// DMX HARDWARE KONFIGURATION
// ============================================================================

// UART Pinbelegung
#define DMX1_TX_PIN 9
#define DMX1_RX_PIN 10
#define DMX1_EN_PIN 27

#define DMX2_TX_PIN 16
#define DMX2_RX_PIN 17
#define DMX2_EN_PIN 26

#define DMX3_TX_PIN 4
#define DMX3_RX_PIN 5
#define DMX3_EN_PIN 25

#define DMX4_TX_PIN 32
#define DMX4_RX_PIN 33
#define DMX4_EN_PIN 14

// DMX Konstanten
#define DMX_BAUDRATE 250000       // Standard DMX512
#define DMX_CHANNELS 512
#define DMX_BREAK_TIME 200        // Mikrosekunden
#define DMX_MAB_TIME 80           // Mark After Break

// Anzahl der aktiven DMX-Ausgänge
// 2 = nur UART1 + UART2
// 4 = alle (benötigt externe Chips für 3+4)
#define DMX_OUTPUTS_ACTIVE 2

// ============================================================================
// LED STATUS PINS (RGB)
// ============================================================================

#define LED_RED_PIN 32
#define LED_GREEN_PIN 33
#define LED_BLUE_PIN 34

// LED Helligkeitsanpassung (0-255)
#define LED_BRIGHTNESS 255

// LED Blink-Frequenzen (ms)
#define LED_BLINK_FAST 200   // Fehler blinkt schnell
#define LED_BLINK_SLOW 500   // Normal blinkt langsam
#define LED_BLINK_ACTIVE 100 // Daten aktiv

// ============================================================================
// TIMEOUTS & FEHLERBEHANDLUNG
// ============================================================================

// Wie lange ohne Pakete bevor "Timeout" angenommen wird (ms)
#define PACKET_TIMEOUT_MS 5000

// Failsafe: Sende letzten DMX-Wert bei Timeout?
#define FAILSAFE_MODE true

// Auto-Recovery versuchen?
#define AUTO_RECOVERY true

// Recovery Interval (ms)
#define RECOVERY_INTERVAL 10000

// ============================================================================
// DEBUG & LOGGING
// ============================================================================

// Serial Debug Output aktiviert?
#define DEBUG_ENABLED true

// Debug Baud Rate
#define DEBUG_BAUD_RATE 115200

// Welche Informationen loggen?
#define DEBUG_ETHERNET true
#define DEBUG_SACN true
#define DEBUG_ARTNET true
#define DEBUG_DMX true
#define DEBUG_STATS true

// Status-Ausgabe Interval (ms)
#define STATUS_INTERVAL 10000

// ============================================================================
// SICHERHEIT & LIMITS
// ============================================================================

// Maximum Universes (sACN/ArtNet)
#define MAX_UNIVERSES 4

// Maximum Fehler bevor kritischer Alarm
#define ERROR_THRESHOLD 50

// Spannungsüberwachung (optional, wenn ADC aktiviert)
#define MONITOR_VOLTAGE false
#define VOLTAGE_ADC_PIN 35
#define VOLTAGE_THRESHOLD_LOW 4.5f   // Warnung unter 4.5V

// ============================================================================
// PERFORMANCE TUNING
// ============================================================================

// Update-Frequenz (wie oft Status gecheckt wird)
#define UPDATE_RATE_HZ 44  // DMX512 Rate (1000/23ms)

// UDP Buffer Größe
#define UDP_BUFFER_SIZE 1500

// UART RX Buffer
#define UART_RX_BUFFER_SIZE 256

// ============================================================================
// FEATURES (EIN/AUS)
// ============================================================================

// Web-Interface Support
#define ENABLE_WEBSERVER false
#define WEBSERVER_PORT 80

// SD-Karte Logging
#define ENABLE_SD_LOGGING false
#define SD_CS_PIN 5

// OLED Display Support (128x64 I2C)
#define ENABLE_OLED false
#define OLED_SDA 21
#define OLED_SCL 22

// Button für manuelle Tests
#define ENABLE_TEST_BUTTON false
#define TEST_BUTTON_PIN 35

// ============================================================================
// KALIBRIERUNG & ABSTIMMUNG
// ============================================================================

// DMX Start Code (standardmäßig 0x00)
#define DMX_START_CODE 0x00

// Sequenznummer für ArtNet (Auto-Increment oder manuell)
#define ARTNET_AUTO_SEQUENCE true

// sACN Priorität (0-200, Standard: 100)
#define SACN_PRIORITY 100

// ============================================================================
// EXPERIMENTELLE FEATURES
// ============================================================================

// RDM Support (Remote Device Management) - NICHT IMPLEMENTIERT
#define ENABLE_RDM false

// DMX Merge Modi (nicht implementiert)
// 0 = Latest Wins (Standard)
// 1 = HTP (Highest Takes Priority)
#define DMX_MERGE_MODE 0

// Automatischer Universe-Überlauf
// z.B. Universe 0 Kanäle 1-512, Universe 1 Kanäle 513-1024
#define AUTO_UNIVERSE_OVERFLOW false

// ============================================================================
// PROFILING & DIAGNOSE
// ============================================================================

// CPU-Last messen?
#define PROFILE_CPU false

// Speicher-Fragmentation?
#define PROFILE_MEMORY false

// Ethernet-Statistik tracken?
#define PROFILE_NETWORK true

// ============================================================================
// HELPER MACROS
// ============================================================================

#if DEBUG_ENABLED
  #define DEBUG_PRINT(x) Serial.print(x)
  #define DEBUG_PRINTLN(x) Serial.println(x)
  #define DEBUG_PRINTF(...) Serial.printf(__VA_ARGS__)
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
  #define DEBUG_PRINTF(...)
#endif

// Conditional Debug
#define DEBUG_ETH(x) if(DEBUG_ETHERNET) Serial.println(x)
#define DEBUG_SACN(x) if(DEBUG_SACN) Serial.println(x)
#define DEBUG_ARTNET(x) if(DEBUG_ARTNET) Serial.println(x)
#define DEBUG_DMX(x) if(DEBUG_DMX) Serial.println(x)

// ============================================================================
// VALIDIERUNG (Zur Compile-Zeit)
// ============================================================================

#if DMX_OUTPUTS_ACTIVE > 4
  #error "DMX_OUTPUTS_ACTIVE darf max. 4 sein"
#endif

#if PACKET_TIMEOUT_MS < 1000
  #warning "PACKET_TIMEOUT_MS unter 1000ms - könnte zu viele False-Positives geben"
#endif

#if LED_BRIGHTNESS > 255
  #error "LED_BRIGHTNESS muss 0-255 sein"
#endif

// ============================================================================
// ANPASSUNGSANLEITUNG
// ============================================================================

/*
SCHNELLE ANPASSUNGEN:

1. IP-Adresse ändern:
   USE_DHCP = false
   STATIC_IP = IPAddress(192, 168, 1, 50)

2. Debug Output deaktivieren:
   DEBUG_ENABLED = false

3. Nur 2 DMX-Leitungen nutzen:
   DMX_OUTPUTS_ACTIVE = 2

4. Timeout erhöhen (langsame Netzwerke):
   PACKET_TIMEOUT_MS = 10000

5. Failsafe deaktivieren:
   FAILSAFE_MODE = false

6. Web-Interface aktivieren (noch nicht implementiert):
   ENABLE_WEBSERVER = true

DEBUGGING:
- Serial Monitor öffnen (115200 baud)
- Die Ausgaben zeigen alle Probleme an
- Mit test_utilities.cpp erweiterte Tests möglich
*/
