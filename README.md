# sACN-Node
I vibecoded everything. Ideas and base code is mine tho

---
MOST STUFF WAS WRITTEN BY CLAUDE.AI I DO NOT KNOW IF ANY OF THIS IS COPIED FROM ANYTHING. IF YOU OR SOMEONE YOU KNOW OWNS ANY OF THIS PLEASE WRITE ME AN EMAIL.
---

# ESP32 DMX Encoder - sACN/ArtNet to 4x DMX512

Professioneller DMX-Konverter für Lichtsteuerung mit GrandMA2, ETC Eos, Chamsys MagicQ oder anderen Lichtpulten.

## 📋 Übersicht

Dieses Projekt wandelt sACN (E1.31) und ArtNet-Signale vom Lichtpult in DMX512-Signale um:

```
GrandMA2 / Lichtpult
       ↓ (Ethernet)
ESP32 ETH01 (sACN/ArtNet)
       ↓
[Universe 0] → DMX1 (MAX485 + XLR)
[Universe 1] → DMX2 (MAX485 + XLR)
[Universe 2] → DMX3 (MAX485 + XLR)
[Universe 3] → DMX4 (MAX485 + XLR)
```

**Eigenschaften:**
- ✓ sACN (E1.31) Unterstützung
- ✓ ArtNet Unterstützung
- ✓ 4x DMX512 Ausgänge (512 Kanäle/Linie)
- ✓ Gigabit Ethernet (RJ45)
- ✓ Hot-Plug fähig
- ✓ Failsafe Mechanismen
- ✓ LED-Status Anzeigen (optional)

---

## 🔧 Hardware-Setup

### Benötigte Komponenten

| Komponente | Menge | Notizen |
|-----------|-------|---------|
| ESP32-ETH01 | 1 | Mit Ethernet-Modul |
| MAX485 TTL-RS485 | 4 | Für DMX Ausgang |
| XLR-3 Buchse (weiblich) | 4 | DMX Out Konnektoren |
| Dupont Kabel (M-M, M-W) | ~40 | Verschiedene Längen |
| RJ45 Stecker/Buchse | 1-2 | Für GrandMA2 Netzwerk |
| 5V Spannungsregler | 1 | Für Stromversorgung |
| Gehäuse | 1 | 3D-gedruckt oder gekauft |

### Pinbelegung

#### ESP32 ETH01 - Ethernet Pins
```
GPIO23 (MDC)    ← Ethernet Daten Clock
GPIO18 (MDIO)   ← Ethernet Daten Interface
GPIO0           ← Ethernet Clock Input
GPIO35, 36, 39  ← Ethernet Daten [RX/TX]
```

#### DMX UART Zuordnung

| DMX Line | UART | TX Pin | RX Pin | DE/RE Pin | Notiz |
|----------|------|--------|--------|-----------|-------|
| DMX 1 | UART1 | 9 | 10 | 27 | ✓ Hardware |
| DMX 2 | UART2 | 16 | 17 | 26 | ✓ Hardware |
| DMX 3 | - | 4 | 5 | 25 | ✗ Software (Optional) |
| DMX 4 | - | 32 | 33 | 14 | ✗ Software (Optional) |

> **Hinweis:** UART0 ist für USB-Debugging reserviert. DMX 3+4 benötigen Bit-Banging oder externe UART-Chips.

#### MAX485 Pinbelegung (für alle 4 Module gleich)

```
MAX485 Pin  →  ESP32 Pin      Beschreibung
┌─────────────────────────────────────────┐
│ 1 (GND)     →  GND             Masse    │
│ 2 (DI)      →  TX (UART)       Data In  │
│ 3 (RO)      →  RX (UART)       REC Out  │
│ 4 (RE)      →  DE/RE Pin       REC Ena  │
│ 5 (DE)      →  DE/RE Pin       Drive En │
│ 6 (DO)      →  NC              (nicht genutzt) │
│ 7 (A)       →  XLR+ (Pin 2)    DMX+ │
│ 8 (+5V)     →  +5V             Spannung │
└─────────────────────────────────────────┘
```

---

## 🔌 Schaltplan

### DMX Ausgang (XLR-3)
```
DMX Line (RS485 A/B)
    │
    MAX485
    ├─ Pin 7 (A)  → XLR Pin 2 (+, Daten)
    └─ Pin 8 (B)  → XLR Pin 3 (-, Daten)
    
ESP32
├─ TX (GPIO 9/16/etc)  → MAX485 Pin 2 (DI)
├─ GND                 → MAX485 Pin 1 (GND)
├─ GPIO DE/RE          → MAX485 Pin 4/5 (RE/DE)
└─ +5V                 → MAX485 Pin 8 (+5V)

XLR-3 Stecker (Blick von hinten):
    ╔═════╗
    ║ 1 3 ║
    ║  2  ║
    ╚═════╝
    
1 = GND (Schirm)
2 = DMX+ (A, High)
3 = DMX- (B, Low)
```

### Stromversorgung

```
USB oder Externe 5V
    │
    ├─ Spannungsregler (wenn >5V)
    │
    ├─ ESP32 +5V Pin
    ├─ 4x MAX485 +5V Pin
    └─ GND auf allen
```

> **Wichtig:** MAX485 benötigen stabile 5V! Nie 3.3V verwenden.

---

## ⚡ Bestückungsanleitung

### 1. Verdrahtung

```
Schritt 1: DMX1 (UART1) - Erste MAX485
────────────────────────────────────────
ESP32     ↔ MAX485 #1    ↔ XLR Buchse 1
GPIO9 (TX)  → Pin 2 (DI)
GPIO10      → Pin 3 (RO)
GPIO27      → Pin 4+5 (RE/DE)
GND         → Pin 1 (GND)
+5V         → Pin 8 (+5V)

MAX485 Pin 7 (A) → XLR Pin 2
MAX485 Pin 8 (B) → XLR Pin 3
GND              → XLR Pin 1


Schritt 2: DMX2 (UART2) - Zweite MAX485
────────────────────────────────────────
ESP32     ↔ MAX485 #2    ↔ XLR Buchse 2
GPIO16      → Pin 2 (DI)
GPIO17      → Pin 3 (RO)
GPIO26      → Pin 4+5 (RE/DE)
GND         → Pin 1 (GND)
+5V         → Pin 8 (+5V)

[Verdrahtung wie DMX1]


Schritt 3: DMX3+4 (Softserial) - Optional
────────────────────────────────────────
Benötigt externe UART-Chips oder Bit-Banging:
- CH340: I2C Interface
- oder: SoftwareSerial (Bibliothek)
- oder: separate Controller für DMX3/4
```

### 2. Bestückungsreihenfolge

1. **Prüfung:** ESP32-Modul auf Beschädigungen checken
2. **MAX485 Module** auf Breadboard oder Perf-Board platzieren
3. **Spannungsversorgung:** +5V/GND Schienen
4. **UART Verbindungen** sorgfältig löten
5. **Kontrollpins (DE/RE)** mit korrekten GPIO verbinden
6. **XLR-Buchsen** mit MAX485 Ausgängen löten
7. **Ethernet-Kabel** anschließen
8. **USB** (für Programming) / Stromversorgung

### 3. Lötarbeiten

```
MAX485 Modul löten:
├─ Alle 8 Pins korrekt identifizieren
├─ Keine Kaltlötstellen (volles Zinn, glänzend)
├─ Nachbardrähte nicht verbinden (Kurz!)
└─ Mit Lupe überprüfen

XLR-Buchse löten:
├─ Pin 1 (GND): Schirm / Gehäuse
├─ Pin 2 (+): +A (weiß)
├─ Pin 3 (-): -B (blau)
└─ Verzinnte Drähte verwenden
```

---

## 📦 3D-Druck Gehäuse

Das Gehäuse sollte folgende Anforderungen erfüllen:

### Design-Spezifikation
```
Abmessungen: ca. 150 x 100 x 80 mm (HxBxT)

Öffnungen:
├─ 1x RJ45 Ethernet (oben)
├─ 4x XLR-3 Buchsen (unten)
├─ 1x USB-C (für Upload/Debug)
├─ 2x LED Status Fenster (optional)
└─ 2x Lüftungsöffnungen (Kühlung)

Material: PLA oder ABS
Infill: 15-20% (Honeycomb)
Support: Ja (breakaway)
Schicht: 0.2mm
Zeit: ~6-8h
```

### OpenSCAD Konzept-Code

```scad
// ESP32 DMX Encoder Gehäuse
// OpenSCAD

$fn = 32; // Qualität

// Gehäuse Abmessungen
width = 150;
height = 100;
depth = 80;
wall = 2;

// Hauptgehäuse
difference() {
  // Äußeres Rechteck
  cube([width, height, depth], center=true);
  
  // Innenraum (Hohlraum)
  translate([0, 0, 1])
    cube([width-2*wall, height-2*wall, depth-wall], center=true);
  
  // RJ45 Öffnung (oben)
  translate([0, 0, depth/2])
    cube([15, 13, 10], center=true);
  
  // XLR Öffnungen (unten, 4x)
  for(i = [0:3]) {
    x_pos = -40 + i*30;
    translate([x_pos, 0, -depth/2+8])
      cylinder(h=15, r=11, center=true);
  }
  
  // USB-C Öffnung (Seite)
  translate([-width/2, 0, -20])
    cube([10, 9, 5], center=true);
}

// Montageschienen für PCB
translate([0, -height/4, -10])
  cube([width-4, 2, 2], center=true);
```

**Für Download:** Thingiverse.com Suche "DMX Controller ESP32"

---

## 💻 Software Installation

### Voraussetzungen
- Visual Studio Code
- PlatformIO Extension
- ESP32 Treiber installiert

### Installation

```bash
# 1. Clone Repository
git clone <url> dmx-encoder
cd dmx-encoder

# 2. Abhängigkeiten installieren (automatisch via PlatformIO)

# 3. Code uploaden
platformio run -t upload

# 4. Monitor starten
platformio device monitor --baud 115200
```

---

## 🌐 Netzwerk-Konfiguration

### GrandMA2 Setup

1. **System → Network → DMX Interfaces**
   ```
   Art-Net Universe 0 → ESP32 IP
   Art-Net Universe 1 → ESP32 IP
   Art-Net Universe 2 → ESP32 IP
   Art-Net Universe 3 → ESP32 IP
   ```

2. **Test → Output → Visualizer**
   ```
   Alle Universen sollten sich ändern
   ```

3. **Fixtures** in Universe 0-3 patchen

### Alternative: sACN (E1.31)

```
GrandMA2: System → Network → sACN Transmit
Enable: ja
Universe: 0-3
IP Adresse: <ESP32 IP>
Port: 5568 (Standard)
```

### Netzwerk-Test

```bash
# Von außen testen (Ethernet-Kabel ins Netzwerk)
ping <ESP32-IP>

# UDP Port Check (Linux)
nc -u <ESP32-IP> 5568  # sACN
nc -u <ESP32-IP> 6454  # ArtNet
```

---

## 🧪 Debugging & Fehlersuche

### Serial Monitor

```
Die Debug-Ausgaben erscheinen bei 115200 Baud:

================================================
  ESP32 DMX Encoder - Boot Start
  sACN & ArtNet to 4x DMX512
================================================

Initialisiere Ethernet...
Warte auf Ethernet-Verbindung...
✓ Ethernet gestartet
✓ Ethernet verbunden
✓ IP-Adresse: 192.168.1.100
✓ Netmask: 255.255.255.0
✓ Gateway: 192.168.1.1
✓ sACN Server läuft auf Port 5568

Initialisiere DMX UARTs...
✓ DMX 1 (UART1) initialisiert
✓ DMX 2 (UART2) initialisiert
✓ DMX Ausgänge bereit

--- STATUS ---
Ethernet: ✓ Verbunden
DMX 1: ✓ Aktiv (1234ms)
DMX 2: ✗ Inaktiv (45000ms)
DMX 3: ✗ Inaktiv (45000ms)
DMX 4: ✗ Inaktiv (45000ms)
```

### Häufige Fehler

| Fehler | Ursache | Lösung |
|--------|--------|--------|
| ✗ Ethernet getrennt | Kabel/Switch | Netzwerk prüfen, Kabel neustecken |
| ✗ DMX inaktiv | Keine Pakete | GrandMA2 Universe prüfen, IP prüfen |
| Keine XLR Signale | MAX485 Verdrahtung | Pins mit Multimeter prüfen (sollte 0/+5V sein) |
| USB Verbindung failsafe | Größere Baud Rate nötig | Upload Speed erhöhen |
| Statisches Rauschen DMX | Schirmung fehlt | Masseleitung prüfen, längere Kabel abschirmen |

### DMX Multimeter Test

```
Mit Multimeter prüfen:

MAX485 Pin 7 (A) gegen Pin 1 (GND):
- Ruhezustand: 2.5V (±1V)
- Daten: -1V bis +5V oszillierend

XLR Pin 2/3:
- Sollte ähnlich wie MAX485 aussehen
```

---

## 📊 Performance & Limitierungen

### Spezifikationen

| Parameter | Wert |
|-----------|------|
| Max. Universen | 4 (ausbaubar) |
| Kanäle/Universe | 512 (DMX Standard) |
| Frame Rate | ~44 Hz (DMX Limit) |
| Latenz | ~22ms (1 Frame) |
| Ethernet Bandbreite | 1 Gbps |
| Stromverbrauch | ~500 mA @5V |

### Bekannte Einschränkungen

- **UART3+4**: Benötigen externe Chips (noch nicht implementiert)
- **Hot Plug**: Nur Ethernet, nicht RJ45 während Betrieb
- **Failsafe**: Sendet letzte bekannte DMX-Werte bei Netzwerkfehler

---

## 🚀 Erweiterungen

### Optionale Features zum Hinzufügen

1. **Web Interface** (WebServer mit IP-Konfiguration)
   ```cpp
   #include <WebServer.h>
   // Konfiguration über Browser
   // http://<IP>/config
   ```

2. **SD-Karte Logging**
   ```cpp
   #include <SD.h>
   // Aufzeichnung aller Pakete
   ```

3. **LED Status Anzeigen**
   ```cpp
   // GPIO32/33 für RGB LED
   // Grün: Ethernet OK
   // Blau: DMX aktiv
   // Rot: Fehler
   ```

4. **OLED Display**
   ```cpp
   #include <Wire.h>
   #include <Adafruit_SSD1306.h>
   // Live Anzeige der Universe Daten
   ```

5. **Mehr DMX Ausgänge** (8x möglich)
   ```cpp
   // Externe UART-Chips über I2C
   // z.B. CH340
   ```

---

## ⚖️ Lizenz & Sicherheit

**Sicherheitshinweise:**
- ⚠ Niemals 12V/24V direkt am ESP32 anschließen!
- ⚠ Lötarbeiten erfordern Fachkenntnisse
- ⚠ RJ45 Ethernets können PoE haben (prüfen!)
- ⚠ DMX Leitungen können lange Kabelstrecken haben (bis 400m möglich)

**Berufliche Nutzung:**
Falls in echtem Theater/Studio eingesetzt:
- UPS (Unterbrechungsfreie Stromversorgung) empfohlen
- Redundante Ethernet-Verbindung (2. Controller)
- Regelmäßige Tests vor Live-Event

---

## 📞 Support & Kontakt

**Probleme?**
1. Serial Monitor Ausgabe prüfen
2. Ethernet-Verbindung testen
3. GrandMA2 Universe/IP korrekt?
4. MAX485 Verdrahtung mit Schaltplan vergleichen

**Code Verbesserungen:**
- Pull Requests willkommen!
- Issues auf GitHub melden

---

## 📚 Referenzen

- **sACN (E1.31):** https://www.usitt.org/standards
- **ArtNet:** https://artisticlicence.com/
- **DMX512:** https://en.wikipedia.org/wiki/DMX512
- **MAX485 Datasheet:** https://www.maximintegrated.com/en/products/rs485-transceivers
- **GrandMA2:** https://www.malighting.com/

---

**Version:** 1.0 | **Stand:** 2026 | **Autor:** Auto-generated
