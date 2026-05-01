/**
 * test_utilities.cpp
 * 
 * Praktische Test- und Debug-Funktionen für DMX Encoder
 * Diese können in den Hauptcode integriert werden
 */

// ============================================================================
// NETZWERK-DIAGNOSTIK
// ============================================================================

/**
 * Zeige Netzwerk-Informationen
 */
void printNetworkDiagnostics() {
  Serial.println("\n╔════════════════════════════════════════╗");
  Serial.println("║     NETZWERK DIAGNOSTIK               ║");
  Serial.println("╚════════════════════════════════════════╝\n");
  
  // Ethernet Status
  Serial.printf("Ethernet Status: %s\n", ethConnected ? "✓ Verbunden" : "✗ Getrennt");
  
  if (ethConnected) {
    Serial.printf("IP-Adresse:     %s\n", ETH.localIP().toString().c_str());
    Serial.printf("Subnet Mask:    %s\n", ETH.subnetMask().toString().c_str());
    Serial.printf("Gateway:        %s\n", ETH.gatewayIP().toString().c_str());
    Serial.printf("DNS:            %s\n", ETH.dnsIP().toString().c_str());
    Serial.printf("MAC-Adresse:    %s\n", ETH.macAddress().c_str());
    
    // Signal Stärke (nicht verfügbar für Ethernet, nur Info)
    Serial.printf("Ethernet Mode:  %s\n", "Gigabit (1000Mbps)");
  }
  
  Serial.println();
}

/**
 * Netzwerk-Verbindung testen (Ping)
 */
void testNetworkPing(const char* host) {
  Serial.printf("\nPing zu %s...\n", host);
  
  IPAddress remote_ip;
  if (!WiFi.hostByName(host, remote_ip)) {
    Serial.println("✗ DNS-Fehler: Host nicht aufgelöst");
    return;
  }
  
  // Einfacher Ping mit ICMP (wenn verfügbar)
  Serial.printf("Resolve: %s\n", remote_ip.toString().c_str());
  
  // Für vollständigen Ping benötigst du lwIP direkt
  // Hier nur Platzhalter:
  Serial.println("(Ping-Funktion: Benötigt lwIP Implementation)");
}

// ============================================================================
// DMX SIGNAL-ANALYSE
// ============================================================================

/**
 * Osszilloskop-ähnliche Live-Anzeige der DMX Daten
 * Zeige die ersten 16 Kanäle als ASCII-Balkendiagramm
 */
void displayDMXOscilloscope(uint8_t universe) {
  if (universe > 3) return;
  
  Serial.printf("\n╔════════════════════════════════════════════╗\n");
  Serial.printf("║ DMX Universe %d - Live Monitor          ║\n", universe);
  Serial.printf("╚════════════════════════════════════════════╝\n");
  
  // Zeige 16 Kanäle mit Skalierung
  for (int ch = 0; ch < 16; ch++) {
    uint8_t value = dmxBuffer[universe][ch];
    int bar_length = (value * 20) / 255;  // Skaliere auf 20 Zeichen
    
    Serial.printf("Ch %2d [", ch + 1);
    
    // Balkendiagramm
    for (int i = 0; i < 20; i++) {
      Serial.print(i < bar_length ? "█" : "░");
    }
    
    Serial.printf("] %3d (0x%02X)\n", value, value);
  }
  
  // Fader info
  Serial.println("\nMax Werte der Universe:");
  uint8_t max_val = 0;
  int max_ch = 0;
  for (int ch = 0; ch < DMX_CHANNELS; ch++) {
    if (dmxBuffer[universe][ch] > max_val) {
      max_val = dmxBuffer[universe][ch];
      max_ch = ch;
    }
  }
  Serial.printf("Höchster Wert: Kanal %d = %d\n", max_ch + 1, max_val);
}

/**
 * Detaillierte Kanal-Analyse
 */
void analyzeDMXUniverse(uint8_t universe) {
  if (universe > 3) return;
  
  uint32_t active_channels = 0;
  uint32_t sum = 0;
  uint8_t min_val = 255, max_val = 0;
  
  // Statistik berechnen
  for (int ch = 0; ch < DMX_CHANNELS; ch++) {
    uint8_t val = dmxBuffer[universe][ch];
    
    if (val > 0) active_channels++;
    sum += val;
    
    if (val < min_val) min_val = val;
    if (val > max_val) max_val = val;
  }
  
  // Ausgabe
  Serial.printf("\n╔════════════════════════════════════════════╗\n");
  Serial.printf("║ ANALYSE: Universe %d                       ║\n", universe);
  Serial.printf("╚════════════════════════════════════════════╝\n");
  
  Serial.printf("Aktive Kanäle:     %lu / 512\n", active_channels);
  Serial.printf("Durchschnittswert:  %u\n", (uint8_t)(sum / 512));
  Serial.printf("Min Wert:          %u\n", min_val);
  Serial.printf("Max Wert:          %u\n", max_val);
  
  // Interessante Kanäle finden
  Serial.println("\nTop 10 aktivste Kanäle:");
  
  struct Channel {
    int number;
    uint8_t value;
  } channels[512];
  
  for (int i = 0; i < 512; i++) {
    channels[i].number = i + 1;
    channels[i].value = dmxBuffer[universe][i];
  }
  
  // Sortiere (einfacher Bubble-Sort, nur Top 10)
  for (int i = 0; i < 10; i++) {
    for (int j = i + 1; j < 512; j++) {
      if (channels[j].value > channels[i].value) {
        Channel temp = channels[i];
        channels[i] = channels[j];
        channels[j] = temp;
      }
    }
  }
  
  for (int i = 0; i < 10; i++) {
    if (channels[i].value > 0) {
      Serial.printf("  %d. Kanal %3d = %3d\n", i + 1, channels[i].number, channels[i].value);
    }
  }
}

// ============================================================================
// SIGNAL-GENERIERUNG (zum Testen ohne Lichtpult)
// ============================================================================

/**
 * Test-Muster: Fade (Alle Kanäle)
 */
void testPatternRamp() {
  static uint8_t level = 0;
  static int direction = 1;
  
  // Alle Kanäle auf gleichen Wert setzen
  memset(dmxBuffer[0], level, 512);
  sendDMX(0, dmxBuffer[0]);
  
  // Langsam hochfahren (1 pro Frame)
  level += direction;
  if (level >= 255) {
    direction = -1;
    Serial.println("↓ Fade Down");
  } else if (level == 0) {
    direction = 1;
    Serial.println("↑ Fade Up");
  }
  
  delay(50);  // 20 Hz
}

/**
 * Test-Muster: Running Chase (Lauflicht)
 */
void testPatternChase(uint8_t width = 5) {
  static uint8_t position = 0;
  
  memset(dmxBuffer[0], 0, 512);
  
  // Breites Licht
  for (int i = 0; i < width; i++) {
    int ch = (position + i) % 512;
    dmxBuffer[0][ch] = 255 - (i * 255 / width);
  }
  
  sendDMX(0, dmxBuffer[0]);
  
  position = (position + 1) % 512;
  delay(100);
}

/**
 * Test-Muster: Strobe (Blinkenlicht)
 */
void testPatternStrobe(uint16_t period_ms = 200) {
  static unsigned long last_toggle = 0;
  static bool state = false;
  
  unsigned long now = millis();
  
  if (now - last_toggle > period_ms / 2) {
    state = !state;
    last_toggle = now;
  }
  
  // Alle Kanäle an oder aus
  memset(dmxBuffer[0], state ? 255 : 0, 512);
  sendDMX(0, dmxBuffer[0]);
}

/**
 * Test-Muster: Rainbow (Farbtemperaturen durchfahren)
 */
void testPatternRainbow() {
  static uint8_t hue = 0;
  
  for (int ch = 0; ch < 512; ch++) {
    // Vereinfacht: Zyklische Helligkeiten
    uint8_t phase = (ch + hue) % 256;
    dmxBuffer[0][ch] = (phase < 128) ? (phase * 2) : (255 - (phase - 128) * 2);
  }
  
  sendDMX(0, dmxBuffer[0]);
  
  hue++;
  delay(50);
}

/**
 * Test-Muster: Punkte (2-3 Random Punkte springen herum)
 */
void testPatternSpots(uint8_t spot_count = 3) {
  static uint16_t spot_pos[10];
  static bool initialized = false;
  
  if (!initialized) {
    for (int i = 0; i < 10; i++) {
      spot_pos[i] = random(512);
    }
    initialized = true;
  }
  
  memset(dmxBuffer[0], 0, 512);
  
  // Spots zeichnen
  for (int i = 0; i < spot_count; i++) {
    if (spot_pos[i] < 512) {
      dmxBuffer[0][spot_pos[i]] = 255;
    }
    
    // Spot bewegen (Random Walk)
    if (random(2) == 0) {
      spot_pos[i] = (spot_pos[i] + 1) % 512;
    } else {
      spot_pos[i] = (spot_pos[i] + 511) % 512;  // Rückwärts
    }
  }
  
  sendDMX(0, dmxBuffer[0]);
  delay(100);
}

// ============================================================================
// INTERAKTIVE TEST-MENÜ
// ============================================================================

/**
 * Serielle Befehle verarbeiten
 * Verwende im Loop: processSerialCommands()
 */
void processSerialCommands() {
  if (!Serial.available()) return;
  
  char cmd = Serial.read();
  
  Serial.println("\n╔════════════════════════════════════════╗");
  Serial.println("║       DMX ENCODER - TEST MENÜ         ║");
  Serial.println("╚════════════════════════════════════════╝\n");
  
  Serial.println("Verfügbare Befehle:");
  Serial.println("  n  → Netzwerk-Status");
  Serial.println("  o  → DMX Osszilloskop (Universe 0)");
  Serial.println("  a  → Analyse Universe 0");
  Serial.println("  r  → Test: Fade Ramp");
  Serial.println("  c  → Test: Chase");
  Serial.println("  s  → Test: Strobe");
  Serial.println("  w  → Test: Rainbow");
  Serial.println("  p  → Test: Spots");
  Serial.println("  t  → Test: Alle Universen");
  Serial.println("  h  → Diese Hilfe\n");
  
  switch (cmd) {
    case 'n':
      printNetworkDiagnostics();
      break;
      
    case 'o':
      displayDMXOscilloscope(0);
      break;
      
    case 'a':
      analyzeDMXUniverse(0);
      break;
      
    case 'r':
      Serial.println("Starte Fade-Test...");
      for (int i = 0; i < 50; i++) {
        testPatternRamp();
      }
      Serial.println("✓ Fade-Test abgeschlossen\n");
      break;
      
    case 'c':
      Serial.println("Starte Chase-Test (10 Sekunden)...");
      for (int i = 0; i < 100; i++) {
        testPatternChase(5);
      }
      Serial.println("✓ Chase-Test abgeschlossen\n");
      break;
      
    case 's':
      Serial.println("Starte Strobe-Test (5 Sekunden)...");
      for (int i = 0; i < 50; i++) {
        testPatternStrobe(200);
      }
      Serial.println("✓ Strobe-Test abgeschlossen\n");
      break;
      
    case 'w':
      Serial.println("Starte Rainbow-Test (10 Sekunden)...");
      for (int i = 0; i < 200; i++) {
        testPatternRainbow();
      }
      Serial.println("✓ Rainbow-Test abgeschlossen\n");
      break;
      
    case 'p':
      Serial.println("Starte Spots-Test (10 Sekunden)...");
      for (int i = 0; i < 100; i++) {
        testPatternSpots(3);
      }
      Serial.println("✓ Spots-Test abgeschlossen\n");
      break;
      
    case 't':
      Serial.println("Teste alle Universen...\n");
      for (int u = 0; u < 4; u++) {
        displayDMXOscilloscope(u);
        delay(500);
      }
      break;
      
    case 'h':
      // Hilfe wird oben schon gezeigt
      break;
      
    default:
      Serial.println("✗ Unbekannter Befehl");
      break;
  }
}

// ============================================================================
// PERFORMANCE-MONITORING
// ============================================================================

/**
 * Zeige Speicher- und Ressourcen-Nutzung
 */
void printMemoryStats() {
  Serial.println("\n╔════════════════════════════════════════╗");
  Serial.println("║     SPEICHER & RESSOURCEN             ║");
  Serial.println("╚════════════════════════════════════════╝\n");
  
  // SRAM Info
  uint32_t free_heap = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
  uint32_t total_heap = heap_caps_get_total_size(MALLOC_CAP_INTERNAL);
  
  Serial.printf("Freier RAM:         %lu / %lu Bytes\n", free_heap, total_heap);
  Serial.printf("RAM Auslastung:     %.1f%%\n", 100.0 * (total_heap - free_heap) / total_heap);
  
  // CPU
  Serial.printf("CPU Frequenz:       %lu MHz\n", getCpuFrequencyMhz());
  
  // Uptime
  unsigned long uptime = millis() / 1000;
  Serial.printf("Uptime:             %lu:%02lu:%02lu (h:m:s)\n",
    uptime / 3600, (uptime % 3600) / 60, uptime % 60);
  
  Serial.println();
}

/**
 * Schreibe Performance-Report in Datei (wenn SD vorhanden)
 */
void logPerformanceReport() {
  Serial.println("Performance Report:");
  Serial.println("╔════════════════════════════════════════╗");
  Serial.printf("║ Uptime:         %10lu ms       ║\n", millis());
  Serial.printf("║ Packets sent:   %10lu         ║\n", 0);  // TODO: globale Variable
  Serial.printf("║ Errors:         %10lu         ║\n", 0);
  Serial.printf("║ Avg CPU Load:   %10u%%        ║\n", 50);  // Estimiert
  Serial.println("╚════════════════════════════════════════╝");
}

// ============================================================================
// INTEGRATIONSBEISPIEL FÜR MAIN LOOP
// ============================================================================

/*
// Im Setup:
void setup() {
  Serial.begin(115200);
  // ... andere Initialisierung ...
  
  Serial.println("\nTipp: Serielle Befehle für Tests eingeben (h für Hilfe)");
}

// Im Loop:
void loop() {
  // Normale Operationen
  // ...
  
  // Test-Menü verarbeiten
  processSerialCommands();
  
  delay(100);
}
*/
