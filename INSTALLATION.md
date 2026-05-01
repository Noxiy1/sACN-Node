# DMX Encoder - Praktische Installationsanleitung

## Phase 1: Vorbereitung (30 Min)

### 1.1 Material-Checkliste

- [ ] ESP32-ETH01 Modul
- [ ] 4x MAX485 Module (TTL-RS485)
- [ ] 4x XLR-3 Buchse (weiblich, DMX)
- [ ] 4x 120Ω Abschlusswiderstände (für DMX)
- [ ] RJ45 Ethernet-Kabel
- [ ] USB-C Kabel (für Upload)
- [ ] 5V Stromversorgung (mind. 1A)
- [ ] Dupont-Kabel (verschiedene Längen)
- [ ] Schrumpfschläuche (verschiedene Durchmesser)
- [ ] Lötkolben + Zinn
- [ ] Multimeter
- [ ] Kleine Zange
- [ ] Seitenschneider

### 1.2 Arbeitsplatz vorbereiten

```
✓ ESD-sichere Oberfläche (Matte/Schreibtisch)
✓ Gute Beleuchtung
✓ Lötkolben aufgewärmt (~350°C)
✓ Nasse Schwamm zum Reinigen
✓ Ablageflächen für Komponenten
```

---

## Phase 2: Hardware-Vorbereitung (1 Stunde)

### 2.1 Dupont-Kabel vorbereiten

Für jede DMX-Leitung benötigt du ca. 10-15 Dupont-Kabel:

```
DMX1 Dupont-Liste (wiederholen für DMX2):
├─ TX (ESP32 GPIO9 → MAX485 Pin 2)
├─ RX (ESP32 GPIO10 → MAX485 Pin 3)  [optional]
├─ EN (ESP32 GPIO27 → MAX485 Pin 4+5)
├─ +5V (Quelle → MAX485 Pin 8)
├─ GND (Quelle → MAX485 Pin 1)
└─ 2x Kabel für XLR Verbindung
```

**Kabel vorbereiten:**
1. Aderendhülsen aufziehen
2. In Dupont-Gehäuse stecken
3. Fixierung überprüfen

### 2.2 XLR-Buchsen präparieren

```
XLR-3 Stecker Belegung (Blick von vorne):
    ╔═════╗
    ║ 1 3 ║  (1=GND, 2=+Data, 3=-Data)
    ║  2  ║
    ╚═════╝

Löt-Anschlüsse:
- Pin 1 (GND):    Schwarzes Kabel
- Pin 2 (A, +):   Weißes Kabel
- Pin 3 (B, -):   Blaues Kabel

Achte auf:
✓ Drähte nicht zu lang (10-15cm)
✓ Isolierung beim Löten erhalten
✓ Kaltlötstellen vermeiden
```

### 2.3 Breadboard/Perf-Board Layout

```
Layout Beispiel:

+5V ═════════════════════════════════
 ║                                   ║
[ESP32]    [MAX485#1]    [MAX485#2]
 ║         │ │ │ │ │       │ │ │
 ║         1 2 3 4 5 8     1 2 3 4 5 8
 ║         │ │ │ │ │       │ │ │
GND ═══════╧═╧═╧═╧═╧═══════╧═╧═╧
```

---

## Phase 3: Elektronik Zusammenbau (2-3 Stunden)

### 3.1 MAX485 Module auf Breadboard/Perf-Board

**Für Breadboard (Temporär):**
```
1. MAX485 zentriert auf Breadboard stecken
   ├─ Pin 1-4 auf eine Seite
   └─ Pin 5-8 auf andere Seite

2. Spannungsversorgung:
   ├─ Pin 1 (GND) → GND Schiene
   └─ Pin 8 (+5V) → +5V Schiene

3. Testverbindung mit TX von ESP32
```

**Für Perf-Board (Dauerhaft):**
```
1. MAX485 Pins durch Löcher stecken
2. Nachbarschaft checken (keine Brücken!)
3. Von unten mit Zinn fixieren
4. Nachbardrähte durchschieben
5. Alle Masse-Verbindungen löten
```

### 3.2 Kabel vom ESP32 zum MAX485

**GPIO9 (TX1) → MAX485#1 Pin 2:**
```
1. Dupont-Stecker vom TX-Kabel auf Breadboard
2. MAX485 Pin 2 in gleiche Reihe
3. Überbrückungskabel verbinden
4. Mit Ohm-Meter checken: Kontinuität?
```

**GPIO27 (EN1) → MAX485#1 Pin 4+5:**
```
1. Ein Dupont-Kabel zu Pin 4 UND 5
   ODER zwei separate Kabel mit Jumper
2. Überprüfen, dass beide Pins verbunden
```

**+5V und GND überall:**
```
Jeden MAX485 Chip:
- Pin 1 (GND) zum GND Schiene
- Pin 8 (+5V) zur +5V Schiene
```

### 3.3 XLR-Buchsen anschließen

**Für jede XLR-Buchse:**

```
MAX485 → XLR Verdrahtung:

MAX485 Pin 7 (A, RS485+) → XLR Pin 2 (Weiß)
MAX485 Pin 8 (B, RS485-) → XLR Pin 3 (Blau)
GND                      → XLR Pin 1 (Schirm/Schwarz)

Löt-Reihenfolge:
1. Pin 1 (GND):   Schwarzes Kabel verlöten
2. Pin 2 (+):     Weißes Kabel verlöten
3. Pin 3 (-):     Blaues Kabel verlöten
4. Mit Heißkleber stabilisieren
5. Schrumpfschlauch über Lötstellen
```

**Überprüfung mit Multimeter:**
```
Ohm-Meter Messungen (XLR sollte sein):
- Pin 1-2: Unbegrenzt (∞ Ohm)
- Pin 1-3: Unbegrenzt (∞ Ohm)
- Pin 2-3: Unbegrenzt (∞ Ohm)

Die Schirme sollten leitend sein:
- XLR Pin 1 zur Masse
```

---

## Phase 4: Gehäuse & Integration (1-2 Stunden)

### 4.1 3D-Druck des Gehäuses

```
OpenSCAD STL-Export:
1. File → Export as STL
2. Slicing (Cura/PrusaSlicer):
   ├─ 0.2mm Layer Height
   ├─ 20% Infill (Honeycomb)
   ├─ Support: Ja
   └─ Zeit: ~6-8h (bei 100mm/s)

3. Nach dem Druck:
   ├─ Support entfernen
   ├─ Rauhe Kanten abschleifen
   └─ Test-Fit machen
```

### 4.2 Komponenten ins Gehäuse

```
Installation:

1. Montageleisten für Perf-Board kleben
   ├─ 4x kleine Kunststoff-Ständer
   └─ Mit Zwei-Komponenten-Kleber fixieren

2. ESP32-Modul anschrauben
   ├─ M3 Schrauben + Unterlegscheiben
   └─ Nicht zu fest anziehen!

3. XLR-Buchsen in Öffnungen pressen
   ├─ Sollten eng sitzen
   └─ Mit kleinen Schrauben von innen optional sichern

4. RJ45 Ethernet-Anschluss
   ├─ Kabel durchführen
   └─ Mit Kabelklemme sichern

5. Spannungsverteilung
   ├─ +5V/GND Schiene anbringen
   └─ Alle Verbraucher anschließen
```

### 4.3 Stromversorgung

```
Externe 5V Stromversorgung:
┌─────────────────────────┐
│ USB Power Supply 5V/2A  │
│ oder                    │
│ DIN-Netzteiler 5V/2A    │
└────────┬────────────────┘
         │
    ┌────┴────┐
    │          │
   +5V        GND
    │          │
    ├─────┬────┘
    │     │
   (+)   (-)
    │     │
  [Schiene im Gehäuse]
    │     │
    ├─────┼─── ESP32
    ├─────┼─── MAX485#1
    ├─────┼─── MAX485#2
    └─────┴─── MAX485#3+4
```

**Sicherheit:**
```
✓ Alle GND verbunden
✓ +5V kein offenes Kabel
✓ Stromstärke prüfen (max 2A für alle)
✓ Kurze, dicke Drähte verwenden
```

---

## Phase 5: Software Installation (30 Min)

### 5.1 VS Code + PlatformIO

```
Installation Windows/Mac/Linux:

1. VS Code herunterladen (code.visualstudio.com)
2. PlatformIO Extension installieren
   ├─ VS Code → Extensions (Ctrl+Shift+X)
   ├─ "PlatformIO" suchen
   └─ Install klicken

3. Projekt öffnen
   ├─ PlatformIO Home
   ├─ "Open Project"
   └─ dmx-encoder Folder wählen
```

### 5.2 Code Upload

```
1. USB-C Kabel ESP32 ↔ Computer

2. In VS Code:
   ├─ Source-Code editieren
   ├─ [Ctrl+Alt+U] = Build & Upload
   └─ Warten auf "SUCCESS"

3. Serial Monitor öffnen
   ├─ [Ctrl+Alt+S] oder
   ├─ Unten: "PlatformIO → Device Monitor"
   └─ Sollte Boot-Ausgaben zeigen
```

### 5.3 Boot-Test

```
Serial Monitor bei 115200 Baud sollte zeigen:

================================================
  ESP32 DMX Encoder - Boot Start
  sACN & ArtNet to 4x DMX512
================================================

Initialisiere Ethernet...
Warte auf Ethernet-Verbindung...
✓ Ethernet gestartet
✓ Ethernet verbunden
✓ IP-Adresse: 192.168.1.100

[SUCCESS] → Weitermachen!
[FEHLER]  → Siehe Phase 6 (Debugging)
```

---

## Phase 6: Testing & Inbetriebnahme (1-2 Stunden)

### 6.1 Grundtest ohne Lichtpult

```
Test-Setup:
1. Ethernet-Kabel ins Netzwerk
2. Osszilloskop an XLR Pin 2+3 anschließen
3. Serial Monitor öffnen

Erwartet:
- Ethernet verbunden ✓
- Keine DMX Ausgaben (kein Puls vorhanden)
```

### 6.2 Mit Testsoftware (z.B. Madrix Light Editor)

```
Kostenlose Test-Software:
- Madrix Light Editor (Free Demo)
- DMXControl 3
- QLC+ (Open Source)

Setup:
1. Software auf Computer installieren
2. ArtNet/sACN konfigurieren:
   IP: <ESP32 IP>
   Universe: 0
3. Kanal 1-10 auf volle Helligkeit
4. Serial Monitor sollte zeigen:
   "ArtNet Universe 0 -> DMX 1 ✓"
```

### 6.3 Mit GrandMA2 Commandwing

```
GrandMA2 Setup:

System → Network → DMX Interfaces:
┌──────────────────────────────┐
│ Art-Net Transmit             │
│ ✓ Enabled                    │
│                              │
│ Universe 0: <ESP32 IP>       │
│ Universe 1: <ESP32 IP>       │
│ Universe 2: <ESP32 IP>       │
│ Universe 3: <ESP32 IP>       │
│                              │
│ [Test] [Save]                │
└──────────────────────────────┘

Test durchführen:
1. Konsole: "Patch 1.1 Dimmer"
2. Fader auf 100%
3. Osszilloskop sollte DMX Puls zeigen
4. Serial Monitor: "ArtNet Universe 0 -> DMX 1 ✓"
```

### 6.4 Vollständiger Funktionstest

```
Checkliste für Live-Inbetriebnahme:

Ethernet:
 ☐ Ping <ESP32-IP> erfolgreich
 ☐ Licht des RJ45-Buchse aktiv
 ☐ Lichtpult: Netzwerk-Diagnose ✓

sACN/ArtNet:
 ☐ Pakete werden empfangen
 ☐ Serial Monitor: Aktive Universe
 ☐ IP-Adresse korrekt konfiguriert

DMX Ausgänge:
 ☐ Alle 4 XLR-Buchsen testen
 ☐ Mit Osszilloskop verifizieren
 ☐ Mit echten DMX-Geräten testen

Performance:
 ☐ Keine Dropouts bei maximaler Last
 ☐ Lichtpult: Output-Tests erfolgreich
 ☐ Recovery nach Ethernet-Fehler

Sicherheit:
 ☐ Stromversorgung stabil
 ☐ Gehäuse geschlossen
 ☐ Alle Kabel befestigt
```

---

## Phase 7: Fehlerbehebung

### Problem: Ethernet nicht verbunden

```
Diagnose:
1. Ethernet-Kabel prüfen (LED am RJ45?)
2. Switch/Router prüfen (andere Geräte funktionieren?)
3. ESP32 Neustarten (Reset-Knopf)
4. Netzwerk-Scan: ping <IP> von Computer

Lösung:
- In Code: IP manuell setzen (DHCP deaktivieren)
- Netzwerk-Kabel Qualität prüfen
- ESP32 an anderen Port probieren
```

### Problem: DMX keine Signale

```
Diagnose:
1. MAX485 Enable-Pin: Multimeter prüfen (HIGH beim Senden)
2. TX Pin: Osszilloskop an GPIO9/16 → sollte 3.3V Puls
3. Spannungsversorgung: +5V vorhanden?

Lösung:
- MAX485 Pin 7/8 überprüfen: 0/5V oszillierend?
- XLR Verdrahtung: Weiß/Blau getauscht?
- Enable-GPIO in Code korrekt?
```

### Problem: Paketfehler/Timeout

```
Diagnose:
1. Serial Monitor: "DMX inaktiv"
2. Lichtpult: Paketrate prüfen
3. Netzwerk: ARP-Tisch prüfen

Lösung:
- Universe in Lichtpult prüfen (0-3)
- sACN/ArtNet richtige Port (5568/6454)
- Multicast-Adresse: 239.69.255.255
```

---

## Checkliste: Projekt abgeschlossen

- [ ] Alle 4 DMX-Leitungen verdrahtet
- [ ] XLR-Buchsen funktionieren
- [ ] Ethernet-Verbindung stabil
- [ ] sACN und ArtNet Tests erfolgreich
- [ ] Mit Lichtpult getestet (mindestens 1 Universe)
- [ ] Gehäuse zusammengebaut
- [ ] Stromversorgung stabil
- [ ] Dokumentation aktualisiert
- [ ] Recovery-Mechanismen getestet

---

## Live-Betrieb Tipps

```
Vor jedem Show:
1. Equipment 30 Min früher anschalten (Warmup)
2. Network Test: Alle Universen
3. DMX-Ausgänge: Mit Messgerät prüfen
4. Recovery: Notfall-Lichtpult vorbereitet
5. Backup-Stromversorgung (UPS) optional

Während Show:
1. Serial Monitor im Hintergrund laufen
2. Temperatur: Gehäuse sollte warm (nicht heiß) sein
3. Bei Fehler: Ethernet neustecken oder Controller Reset

Nach Show:
1. Ordnungsgemäß herunterfahren (nicht einfach Strom weg!)
2. Logs überprüfen (Error Count)
3. Eventuelle Überhitzung prüfen
```

---

**Version:** 1.0 | **Stand:** 2026
**Alle Angaben ohne Gewähr | Elektronik-Grundkenntnisse erforderlich**
