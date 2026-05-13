# WT32-ETH01 - DMX Encoder Setup

## 🎯 Warum WT32-ETH01 die bessere Wahl ist

### Hardware Vergleich

```
┌─────────────────────────────────────┐
│ Option 1: ESP32 + CH340 (alt)       │
├─────────────────────────────────────┤
│ ESP32 Modul           ~15€           │
│ CH340 Ethernet Modul  ~20€           │
│ Verbindungskabel      ~5€            │
│ Gesamtkomplexität     HOCH           │
│ Zuverlässigkeit       OK             │
│ GPIO-Verfügbarkeit    BEGRENZT       │
│ Kosten Gesamt         ~40€           │
└─────────────────────────────────────┘

┌─────────────────────────────────────┐
│ Option 2: WT32-ETH01 (NEU, BESSER!) │
├─────────────────────────────────────┤
│ WT32-ETH01 Modul      ~18€           │
│ (Ethernet integriert!)               │
│ Verbindungskabel      ~2€            │
│ Gesamtkomplexität     NIEDRIG        │
│ Zuverlässigkeit       AUSGEZEICHNET  │
│ GPIO-Verfügbarkeit    MAXIMAL        │
│ Kosten Gesamt         ~20€           │
│                                      │
│ ✓ 50% billiger!                      │
│ ✓ Einfacher!                         │
│ ✓ Zuverlässiger!                     │
└─────────────────────────────────────┘
```

### Interne Architektur

**ESP32 + CH340 (extern):**
```
┌──────────────┐
│   ESP32      │
│   - Ethernet │─── GPIO Pins spannungsintensiv
│   - 3x UART  │
│   - SMD Chip │
└──────────────┘
       │ (Kabel)
┌──────────────┐
│   CH340      │
│   LAN8720    │ ← externe Hardware
│   PHY        │
└──────────────┘
```

**WT32-ETH01 (integriert):**
```
┌────────────────────────────┐
│     WT32-ETH01 Modul       │
├────────────────────────────┤
│ ┌────────────────────────┐ │
│ │  ESP32 Chip            │ │
│ │  - 3x UART             │ │
│ │  - Alle GPIO           │ │
│ └────────────────────────┘ │
│ ┌────────────────────────┐ │
│ │  LAN8720 PHY           │ │
│ │  (Ethernet INTERN)     │ │
│ └────────────────────────┘ │
│                            │
│ ✓ Alles auf einem PCB      │
│ ✓ Keine externen Chips     │
│ ✓ Stabilere Verbindungen   │
└────────────────────────────┘
```

---

## 🔧 Pinbelegung WT32-ETH01

### Ethernet Pins (INTERN, nicht zu verdrahten!)

```
Die WT32-ETH01 Ethernet Pins sind INTERN verschaltet:
GPIO0   → Ethernet Clock (intern)
GPIO23  → ETH MDC (intern)
GPIO18  → ETH MDIO (intern)
GPIO35-39 → Ethernet RX/TX (intern)

⚠️ DU BRAUCHST DICH NICHT KÜMMERN!
Die Ethernet-Pins sind bereits auf dem Modul verdrahtet.

Code:
ETH.begin(0, -1, 23, 18, ETH_PHY_LAN8720, ETH_CLOCK_GPIO0_IN);
                  ↑  ↑
                  MDC MDIO (fest verdrahtet)
```

### Verfügbare GPIO für DMX & andere Features

```
GPIO Verfügbarkeit auf WT32-ETH01:

Reserviert (Ethernet intern):
├─ GPIO0   (Clock)
├─ GPIO18  (MDIO)
├─ GPIO23  (MDC)
└─ GPIO35-39 (Ethernet RX/TX)

Verfügbar für DMX (3 Hardware UART):
├─ UART0: GPIO1 (TX), GPIO3 (RX)
├─ UART1: GPIO9 (TX), GPIO10 (RX)
└─ UART2: GPIO16 (TX), GPIO17 (RX)

Enable Pins (für MAX485):
├─ GPIO27 (DMX1)
├─ GPIO26 (DMX2)
├─ GPIO25 (DMX3)
└─ GPIO14 (DMX4)

Noch frei für Extras:
├─ GPIO2, GPIO4, GPIO5
├─ GPIO12, GPIO13, GPIO15
├─ GPIO21, GPIO22 (I2C!)
├─ GPIO32, GPIO33 (ADC/LED)
├─ GPIO34, GPIO36, GPIO39 (ADC)
└─ Viele weitere!

→ WT32 hat DOPPELT so viele freie GPIO wie ESP32 + Ethernet!
```

---

## 📋 Wichtige Unterschiede im Code

### WT32-spezifische Ethernet Initialisierung

```cpp
// WT32-ETH01 Ethernet Setup (VEREINFACHT!)
void initEthernetWT32() {
  WiFi.onEvent(ethEvent);
  
  ETH.begin(
    0,                        // LAN8720 Address (fest 0)
    -1,                       // Power Pin nicht nötig
    23,                       // MDC (fest)
    18,                       // MDIO (fest)
    ETH_PHY_LAN8720,         // PHY Typ (fest!)
    ETH_CLOCK_GPIO0_IN       // Clock Mode (fest!)
  );
}

// Vs. ESP32 + extern (komplizierter):
void initEthernetESP32() {
  // Externe Pins definieren
  // Externe Chips kontrollieren
  // Mehr Fehlerquellen
}
```

### Kein Konflikt mit Serial Debug

```cpp
// WT32: GPIO1/3 sind für DMX3 frei!
uart0.begin(250000, SERIAL_8N2, 3, 1);  // ✓ DMX3 über UART0

// ESP32 + CH340: GPIO1/3 möglicherweise belegt
// Serial.begin() könnte konfligieren
```

---

## 🔌 Hardware-Aufbau für WT32

### Minimales Setup (nur DMX, keine extras)

```
WT32-ETH01
├─ +5V (Netzteil) ────────────┐
│                              │
├─ GND (Netzteil) ────────────┐├─ zu allen MAX485
│                              │
├─ RJ45 Ethernet (zum Pult)   │
│                              │
├─ GPIO9 ───────→ MAX485 #1   │
├─ GPIO16 ──────→ MAX485 #2   │
├─ GPIO1 ───────→ MAX485 #3   │
├─ GPIO4 ───────→ MAX485 #4   │
│                              │
├─ GPIO27, 26, 25, 14 ──→ Enable Pins (zu MAX485)
│
└─ Fertig! ✓

(Nur 8 Verbindungskabel statt 20+ beim ESP32 Setup)
```

### Mit Status-LEDs (optional)

```
WT32-ETH01
├─ ... [DMX Verdrahtung wie oben] ...
│
├─ GPIO32 ──→ LED R (rot)
├─ GPIO33 ──→ LED G (grün)
├─ GPIO34 ──→ LED B (blau)
│
└─ Fertig! ✓

(GPIO32-34 sind komplett frei, perfekt für LEDs!)
```

---

## 🌐 Ethernet-Besonderheiten WT32

### DHCP vs Statische IP

```cpp
// WT32 mit DHCP (Standard)
ETH.begin(...);
// → IP wird automatisch vergeben
// Prüfe mit:
Serial.println(ETH.localIP());

// WT32 mit statischer IP (falls nötig)
ETH.config(
  IPAddress(192, 168, 1, 100),  // IP
  IPAddress(192, 168, 1, 1),    // Gateway
  IPAddress(255, 255, 255, 0),  // Netmask
  IPAddress(8, 8, 8, 8)         // DNS
);
ETH.begin(...);
```

### Ethernet Reset bei Problemen

```cpp
// Falls Ethernet hängt:
void resetEthernet() {
  ETH.disconnect(true);   // Hard reset
  delay(1000);
  ETH.begin(...);         // Neustarten
}

// Oder: Wenn WT32 Reset-Pin (GPIO12) hat:
#define ETH_RESET_PIN 12
digitalWrite(ETH_RESET_PIN, LOW);
delay(100);
digitalWrite(ETH_RESET_PIN, HIGH);
```

---

## ⚡ Stromversorgung

### Minimale Anforderung

```
+5V / 1A reicht aus für:
├─ WT32-ETH01    (~200mA)
├─ 4x MAX485     (~80mA)
├─ Status LEDs   (~100mA optional)
└─ Summe: ~380mA (mit Sicherheitsmarge)
```

### Empfohlen

```
+5V / 2A für:
✓ Kopffreiheit bei Spitzenlast
✓ Längere Ethernet-Kabel (Spannungsabfall)
✓ Zusätzliche Features (OLED, etc.)
```

---

## 🧪 Test-Checkliste für WT32

```
Hardware Test:
☐ WT32 Modul eingebaut
☐ Ethernet RJ45 angesteckt (LED sollte blinken!)
☐ Alle 4 MAX485 verdrahtet
☐ +5V und GND überall verbunden
☐ Enable Pins (GPIO27, 26, 25, 14) angeschlossen

Software Test:
☐ dmx_encoder_WT32.cpp hochgeladen
☐ Kein Serial.begin() vorhanden ✓
☐ Kompiliert fehlerfrei ✓

Funktions-Test:
☐ Ethernet verbunden (LED blinkt?)
☐ Mit GrandMA2 verbunden
☐ Universe 0 konfiguriert
☐ Fader auf 100% → DMX sollte aktiv sein
☐ Alle 4 Universen testen (0-3)
☐ Status LED (wenn aktiviert) = BLAU bei Aktivität

Produktion Ready:
✓ Alles funktioniert
✓ Gehäuse zusammengebaut (optional)
✓ Mit echten DMX-Geräten getestet
→ LIVE BEREIT! 🚀
```

---

## 💡 Vorteile bei Zusatz-Features

Da WT32 viel mehr GPIO hat, sind diese Features einfach:

### Status-OLED Display (I2C)

```cpp
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display(128, 64, &Wire, -1);

void setup() {
  Wire.begin(21, 22);  // SDA, SCL (freie Pins auf WT32!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
}

// Zeige DMX Status live an
void displayStatus() {
  display.clearDisplay();
  display.println("DMX Encoder");
  display.printf("DMX1: %s\n", packetCount[0] > 0 ? "✓" : "✗");
  display.printf("DMX2: %s\n", packetCount[1] > 0 ? "✓" : "✗");
  display.printf("DMX3: %s\n", packetCount[2] > 0 ? "✓" : "✗");
  display.printf("DMX4: %s\n", packetCount[3] > 0 ? "✓" : "✗");
  display.display();
}
```

### Zusätz-Buttons

```cpp
#define BUTTON_RESET 2    // Reset Ethernet
#define BUTTON_TEST 5     // Test-Modus

void setup() {
  pinMode(BUTTON_RESET, INPUT_PULLUP);
  pinMode(BUTTON_TEST, INPUT_PULLUP);
}

void loop() {
  if (digitalRead(BUTTON_RESET) == LOW) {
    resetEthernet();
  }
  if (digitalRead(BUTTON_TEST) == LOW) {
    // Test-Patterns senden
    for (int i = 0; i < 512; i++) {
      dmxBuffer[0][i] = i % 256;
    }
    sendDMX(0, dmxBuffer[0]);
  }
}
```

### Temperatur-Monitoring

```cpp
#define TEMP_SENSOR_PIN 34  // ADC (Thermistor)

float readTemperature() {
  int raw = analogRead(TEMP_SENSOR_PIN);
  float voltage = raw * 3.3 / 4096.0;
  // Thermistor Berechnung...
  return temperature;
}
```

---

## 🎓 Zusammenfassung: Warum WT32?

| Kriterium | ESP32+CH340 | WT32-ETH01 |
|-----------|-------------|-----------|
| Ethernet-Chip | extern | **integriert** |
| Komplexität | hoch | **einfach** |
| Kosten | ~40€ | **~18€** |
| Zuverlässigkeit | gut | **exzellent** |
| GPIO verfügbar | 15 | **25+** |
| Setup-Zeit | 2-3h | **30min** |
| Fehlerquellen | viele | **wenige** |
| Zusatz-Features | schwierig | **einfach** |

**Fazit: WT32-ETH01 ist die RICHTIGE Wahl! ✅**

---

## 🚀 Next Steps

1. **Code hochladen:** `dmx_encoder_WT32.cpp`
2. **Ethernet testen:** LED am RJ45 blinkt?
3. **Mit GrandMA2 verbinden:** Universe 0-3 konfigurieren
4. **Alles funktionstüchtig?** → Produzieren! 🎉

**Willkommen zur besseren Lösung!** 🎊
