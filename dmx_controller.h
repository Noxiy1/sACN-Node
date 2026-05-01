/**
 * dmx_controller.h
 * 
 * Erweiterte DMX-Controller Klasse mit:
 * - Status LEDs
 * - Fehlerbehandlung & Recovery
 * - Statistik & Monitoring
 * - Konfigurationsmanagement
 */

#pragma once

#include <Arduino.h>
#include <HardwareSerial.h>

/**
 * Status-LED Pins (RGB)
 * Grün   = Normale Operation
 * Blau   = DMX aktiv
 * Rot    = Fehler
 * Gelb   = Startup
 */
#define STATUS_LED_RED 32
#define STATUS_LED_GREEN 33
#define STATUS_LED_BLUE 34

// ============================================================================
// DMX Controller Klasse
// ============================================================================

class DMXController {
private:
  // UART Konfiguration
  HardwareSerial* uart[2];
  uint8_t uart_tx_pins[2];
  uint8_t uart_rx_pins[2];
  uint8_t uart_en_pins[2];
  
  // DMX Buffer
  uint8_t dmx_buffer[512];
  uint16_t dmx_universe;
  
  // Status Tracking
  unsigned long last_packet_time;
  unsigned long last_error_time;
  uint32_t packet_count;
  uint32_t error_count;
  
  // LED Status
  bool led_blink_state;
  unsigned long last_led_update;
  
  // Konfiguration
  struct Config {
    uint32_t baudrate;
    uint16_t frame_rate;
    uint16_t timeout_ms;
    bool error_recovery;
  } config;
  
public:
  // Konstruktor
  DMXController(uint8_t uart_num) : 
    uart_en_pins{27, 26},
    dmx_universe(0),
    last_packet_time(0),
    last_error_time(0),
    packet_count(0),
    error_count(0),
    led_blink_state(false),
    last_led_update(0)
  {
    // Standard-Konfiguration
    config.baudrate = 250000;
    config.frame_rate = 44;
    config.timeout_ms = 5000;
    config.error_recovery = true;
    
    // UART Zuordnung
    if (uart_num == 0) {
      uart[0] = &Serial1;
      uart_tx_pins[0] = 9;
      uart_rx_pins[0] = 10;
      uart_en_pins[0] = 27;
    } else if (uart_num == 1) {
      uart[0] = &Serial2;
      uart_tx_pins[0] = 16;
      uart_rx_pins[0] = 17;
      uart_en_pins[0] = 26;
    }
  }
  
  /**
   * Initialisiere DMX Controller
   */
  void begin() {
    // UART initialisieren
    uart[0]->begin(config.baudrate, SERIAL_8N2, 
                   uart_rx_pins[0], uart_tx_pins[0]);
    
    // Enable Pin
    pinMode(uart_en_pins[0], OUTPUT);
    digitalWrite(uart_en_pins[0], LOW);  // Empfang
    
    // LEDs
    initStatusLED();
    
    // Buffer löschen
    memset(dmx_buffer, 0, sizeof(dmx_buffer));
    
    setStatusLED(LED_YELLOW);  // Startup
  }
  
  /**
   * Sende DMX512 Signal
   */
  void sendDMX(const uint8_t* data) {
    if (!data) return;
    
    // Copy Data
    memcpy(dmx_buffer, data, 512);
    
    // MAX485 auf SENDEN
    digitalWrite(uart_en_pins[0], HIGH);
    delayMicroseconds(100);
    
    // DMX Break & MAB
    sendDMXBreak();
    
    // Start Code
    uart[0]->write(0x00);
    
    // 512 Bytes Daten
    uart[0]->write(dmx_buffer, 512);
    uart[0]->flush();
    
    // Zurück auf EMPFANG
    digitalWrite(uart_en_pins[0], LOW);
    
    // Update Statistik
    last_packet_time = millis();
    packet_count++;
  }
  
  /**
   * Update Statusanzeige
   */
  void updateStatus() {
    unsigned long now = millis();
    
    // LED Update alle 200ms
    if (now - last_led_update < 200) return;
    last_led_update = now;
    
    // Prüfe Timeout
    unsigned long time_since_packet = now - last_packet_time;
    
    if (time_since_packet > config.timeout_ms) {
      // Fehler: Keine Pakete
      setStatusLED(LED_RED);
      
      if (config.error_recovery) {
        recover();
      }
      error_count++;
      
    } else if (time_since_packet < 100) {
      // Aktiv: Blau blinken
      led_blink_state = !led_blink_state;
      setStatusLED(led_blink_state ? LED_BLUE : LED_BLUE_DIM);
      
    } else {
      // Normal: Grün
      setStatusLED(LED_GREEN);
    }
    
    // Periodisches Logging
    static unsigned long last_log = 0;
    if (now - last_log > 10000) {
      last_log = now;
      logStats();
    }
  }
  
  /**
   * Fehlerbehandlung & Recovery
   */
  void recover() {
    Serial.println("⚠ DMX Controller Recovery gestartet");
    
    // UART Reset
    uart[0]->end();
    delay(500);
    uart[0]->begin(config.baudrate, SERIAL_8N2,
                   uart_rx_pins[0], uart_tx_pins[0]);
    
    // Buffer löschen
    memset(dmx_buffer, 0, 512);
    
    // Kurzes Blinken für Bestätigung
    for (int i = 0; i < 3; i++) {
      setStatusLED(LED_RED);
      delay(100);
      setStatusLED(LED_OFF);
      delay(100);
    }
    
    Serial.println("✓ Recovery abgeschlossen");
  }
  
  /**
   * Debug: Statistik ausgeben
   */
  void logStats() {
    Serial.printf("DMX Stats | Packets: %lu | Errors: %lu | Uptime: %lums\n",
      packet_count, error_count, millis());
    
    // Erste 10 Kanäle anzeigen
    Serial.print("DMX [1-10]: ");
    for (int i = 0; i < 10; i++) {
      Serial.printf("%3d ", dmx_buffer[i]);
    }
    Serial.println();
  }
  
  // Getter
  uint32_t getPacketCount() const { return packet_count; }
  uint32_t getErrorCount() const { return error_count; }
  uint8_t getChannel(uint16_t channel) const {
    if (channel < 512) return dmx_buffer[channel];
    return 0;
  }
  
private:
  
  /**
   * DMX Break Signalisierung
   * Schon geleert auf TX-Low für mindestens 88µs
   */
  void sendDMXBreak() {
    // UART Break senden (wenn verfügbar)
    // Fallback: Kurze Verzögerung
    delayMicroseconds(200);
  }
  
  /**
   * Status LED Initialisierung
   */
  void initStatusLED() {
    pinMode(STATUS_LED_RED, OUTPUT);
    pinMode(STATUS_LED_GREEN, OUTPUT);
    pinMode(STATUS_LED_BLUE, OUTPUT);
    
    digitalWrite(STATUS_LED_RED, LOW);
    digitalWrite(STATUS_LED_GREEN, LOW);
    digitalWrite(STATUS_LED_BLUE, LOW);
  }
  
  /**
   * LED Farben
   */
  enum LEDColor {
    LED_OFF       = 0b000,
    LED_RED       = 0b100,
    LED_GREEN     = 0b010,
    LED_BLUE      = 0b001,
    LED_YELLOW    = 0b110,
    LED_CYAN      = 0b011,
    LED_MAGENTA   = 0b101,
    LED_WHITE     = 0b111,
    LED_BLUE_DIM  = 0b000  // Aus (wechselt mit BLUE)
  };
  
  /**
   * LED Farbe setzen
   */
  void setStatusLED(LEDColor color) {
    digitalWrite(STATUS_LED_RED, (color & 0b100) ? HIGH : LOW);
    digitalWrite(STATUS_LED_GREEN, (color & 0b010) ? HIGH : LOW);
    digitalWrite(STATUS_LED_BLUE, (color & 0b001) ? HIGH : LOW);
  }
  
  // ... weitere private Methoden
};

// ============================================================================
// Globale Controller Instanzen
// ============================================================================

DMXController dmx1(0);  // UART1
DMXController dmx2(1);  // UART2

// ============================================================================
// INITIALISIERUNGSFUNKTIONEN
// ============================================================================

/**
 * Alle DMX Controller starten
 */
void initDMXControllers() {
  dmx1.begin();
  dmx2.begin();
  
  Serial.println("✓ DMX Controller initialisiert");
}

/**
 * Hauptupdated Funktion (im Loop aufrufen)
 */
void updateDMXControllers() {
  dmx1.updateStatus();
  dmx2.updateStatus();
}

// ============================================================================
// UTILITY: Web-basierte Konfiguration (Optional)
// ============================================================================

/**
 * JSON Konfiguration exportieren
 */
void exportConfigJSON() {
  Serial.println("{");
  Serial.println("  \"dmx_channels\": 512,");
  Serial.println("  \"universes\": 4,");
  Serial.printf("  \"baudrate\": %lu,\n", 250000);
  Serial.printf("  \"packets_sent\": %lu,\n", dmx1.getPacketCount());
  Serial.printf("  \"errors\": %lu\n", dmx1.getErrorCount());
  Serial.println("}");
}

// ============================================================================
// Test-Funktionen (für Debugging)
// ============================================================================

/**
 * Test Pattern: Alle Kanäle hochfahren
 */
void testPatternFade() {
  static uint8_t level = 0;
  static int direction = 1;
  
  uint8_t test_data[512];
  memset(test_data, level, 512);
  
  dmx1.sendDMX(test_data);
  
  level += direction;
  if (level >= 255 || level == 0) {
    direction *= -1;
  }
}

/**
 * Test Pattern: Chase (Kanäle nacheinander)
 */
void testPatternChase() {
  static uint8_t position = 0;
  uint8_t test_data[512];
  
  memset(test_data, 0, 512);
  
  // Aktuellen Kanal setzen
  test_data[position] = 255;
  test_data[(position + 1) % 512] = 128;
  test_data[(position + 2) % 512] = 64;
  
  dmx1.sendDMX(test_data);
  
  position = (position + 1) % 512;
}

/**
 * Test Pattern: Strobing
 */
void testPatternStrobe() {
  static unsigned long last_strobe = 0;
  static bool strobe_state = false;
  
  uint8_t test_data[512];
  memset(test_data, strobe_state ? 255 : 0, 512);
  
  dmx1.sendDMX(test_data);
  
  if (millis() - last_strobe > 200) {
    strobe_state = !strobe_state;
    last_strobe = millis();
  }
}
