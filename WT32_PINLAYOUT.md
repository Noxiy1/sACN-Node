# WT32-ETH01 Pinlayout für 4x DMX Encoder

## 📌 Übersicht - alle verwendeten Pins

```
WT32-ETH01 (von oben)

LEFT SIDE (oben):              RIGHT SIDE (oben):
┌─────────────────────────┐    ┌─────────────────────────┐
│ EN        (1)           │    │ 5V        (9)  - Power! │
│ CFG       (2) - unused  │    │ LINK      (10) - optional│
│ 485_EN    (3) - IO33    │    │ GND       (11) - GND    │
│ RXD       (4) - IO5     │    │ IO39      (12) - input  │
│ TXD       (5) - IO17    │    │ IO36      (13) - input  │
│ GND       (6) - GND     │    │ IO15      (14) - free   │
│ 3V3       (7) - Power!  │    │ IO14      (15) - free   │
│ GND       (8) - GND     │    │ IO12      (16) - free   │
└─────────────────────────┘    │ IO35      (17) - input  │
                               │ IO4       (18) - free   │
LEFT SIDE (unten):             │ IO2       (19) - free   │
┌─────────────────────────┐    │ GND       (20) - GND    │
│ IO9       (21) - DMX1TX │    └─────────────────────────┘
│ IO10      (22) - DMX1RX │
│ IO27      (23) - DMX1EN │    RIGHT SIDE (unten):
│ IO26      (24) - DMX2EN │    ┌─────────────────────────┐
│ IO25      (25) - DMX3EN │    │ IO1       (26) - DMX2TX │
│ GND       (27) - GND    │    │ IO3       (27) - DMX2RX │
│ 3V3       (28) - 3.3V   │    │ IO16      (28) - DMX3TX │
│ GND       (29) - GND    │    │ IO17      (29) - DMX3RX │
└─────────────────────────┘    └─────────────────────────┘

Ethernet: RJ45 (integriert) - keine Pins nötig!
```

---

## 🔌 DMX PIN KONFIGURATION

### DMX1 - UART1 (TX9 / RX10)
```
WT32-ETH01           MAX485 Module 1          XLR-3 Stecker
─────────────────────────────────────────────────────────
IO9  (TX) ──────────→ TX (Pin 4)
IO10 (RX) ──────────→ RX (Pin 5)
IO27 (EN) ──────────→ DE/RE (Pin 2,3)
GND ─────────────────→ GND (Pin 8)
Power (3.3V) ──────→ VCC (Pin 1)

MAX485 Output:
A (Pin 6) ──────────→ XLR Pin 3 (Data+)
B (Pin 7) ──────────→ XLR Pin 2 (Data-)
GND ─────────────────→ XLR Pin 1 (Shield)
```

### DMX2 - UART0 (TX1 / RX3)
```
WT32-ETH01           MAX485 Module 2          XLR-3 Stecker
─────────────────────────────────────────────────────────
IO1  (TX) ──────────→ TX (Pin 4)
IO3  (RX) ──────────→ RX (Pin 5)
IO25 (EN) ──────────→ DE/RE (Pin 2,3)
GND ─────────────────→ GND (Pin 8)
Power (3.3V) ──────→ VCC (Pin 1)

MAX485 Output:
A (Pin 6) ──────────→ XLR Pin 3 (Data+)
B (Pin 7) ──────────→ XLR Pin 2 (Data-)
GND ─────────────────→ XLR Pin 1 (Shield)
```

### DMX3 - UART1 Alternative (TX16 / RX17)
```
WT32-ETH01           MAX485 Module 3          XLR-3 Stecker
─────────────────────────────────────────────────────────
IO16 (TX) ──────────→ TX (Pin 4)
IO17 (RX) ──────────→ RX (Pin 5)
IO26 (EN) ──────────→ DE/RE (Pin 2,3)
GND ─────────────────→ GND (Pin 8)
Power (3.3V) ──────→ VCC (Pin 1)

MAX485 Output:
A (Pin 6) ──────────→ XLR Pin 3 (Data+)
B (Pin 7) ──────────→ XLR Pin 2 (Data-)
GND ─────────────────→ XLR Pin 1 (Shield)
```

### DMX4 - UART2 Integrierter RS485 (TX17 / RX5) ⭐ KEINE extra Hardware!
```
WT32-ETH01           Integrierter RS485       XLR-3 Stecker
─────────────────────────────────────────────────────────
IO17 (TXD2) ───────→ (intern verbunden)
IO5  (RXD2) ───────→ (intern verbunden)
IO33 (485_EN) ─────→ (intern gesteuert)

RS485 Ausgang (direkt vom WT32 auf XLR):
RS485_A ────────────→ XLR Pin 3 (Data+)
RS485_B ────────────→ XLR Pin 2 (Data-)
GND ─────────────────→ XLR Pin 1 (Shield)

⚠️ WICHTIG: DMX4 braucht KEIN extra MAX485 Modul!
Die Signale sind bereits auf dem WT32 vorhanden!
```

---

## 🎛️ STATUS LEDs (RGB) - Optional

```
WT32-ETH01           LED Kathode (-)
──────────────────────────────────
IO36 (RED)   ────────→ (100Ω Widerstand) → LED Rot
IO39 (GREEN) ────────→ (100Ω Widerstand) → LED Grün
IO34 (BLUE)  ────────→ (100Ω Widerstand) → LED Blau
GND ─────────────────→ LED Anode (+) / 3.3V

⚠️ NICHT IO32 oder IO33 verwenden!
   IO32: Ethernet Konflikt
   IO33: RS485_EN (DMX4 Enable!)
```

---

## 🔋 STROMVERSORGUNG

```
Externe Stromquelle   WT32-ETH01
───────────────────────────────
+5V ─────────────────→ Pin 9 (5V)   ODER
+3.3V ───────────────→ Pin 7 (3V3)  ← WÄHLE EINS!

GND ─────────────────→ Pin 6, 8, 11, 20, 27, 29 (GND)

⚠️ MINDESTENS 1A Stromversorgung für WT32 + Ethernet!
⚠️ 3.3V ODER 5V - nicht beide!
```

---

## 📊 PIN ZUSAMMENFASSUNG

### Verwendete Pins:

| Pin | Funktion | Verwendung |
|-----|----------|-----------|
| **IO1** | UART0 TX | DMX2 TX |
| **IO3** | UART0 RX | DMX2 RX |
| **IO5** | UART2 RX | DMX4 RX (integriert) |
| **IO9** | UART1 TX | DMX1 TX |
| **IO10** | UART1 RX | DMX1 RX |
| **IO16** | UART1 TX alt | DMX3 TX |
| **IO17** | UART1 RX alt / UART2 TX | DMX3 RX / DMX4 TX |
| **IO25** | GPIO | DMX3 Enable (MAX485) |
| **IO26** | GPIO | DMX2 Enable (MAX485) |
| **IO27** | GPIO | DMX1 Enable (MAX485) |
| **IO33** | GPIO | DMX4 Enable (RS485 intern) ⚠️ |
| **IO34** | GPIO | Status LED Blue |
| **IO36** | GPIO Input | Status LED Red |
| **IO39** | GPIO Input | Status LED Green |
| **GND** | Ground | Masse |
| **3V3** | 3.3V Power | Stromversorgung |
| **5V** | 5V Power | Alternative Stromversorgung |

### Freie Pins (verfügbar für Erweiterungen):

```
IO2, IO4, IO12, IO14, IO15, IO32, IO35
IO36, IO39 (nur Input - für Status LEDs genutzt)
```

---

## 🔗 STECKERBELEGUNG XLR-3 (DMX512)

```
      XLR Buchse (von vorne):

         /‾‾‾‾‾‾‾\
        /   o o   \
       |  o       o |
        \    o    /
         \________/

        Pin 1: GND / Shield
        Pin 2: Data- (B)
        Pin 3: Data+ (A)
```

---

## 📋 VERKABELUNGS-CHECKLISTE

### Vor dem Anschalten:

- [ ] Stromversorgung (3.3V oder 5V - NICHT BEIDE!)
- [ ] GND Verbindungen korrekt
- [ ] DMX1 MAX485 TX/RX/EN/GND/VCC angeschlossen
- [ ] DMX2 MAX485 TX/RX/EN/GND/VCC angeschlossen
- [ ] DMX3 MAX485 TX/RX/EN/GND/VCC angeschlossen
- [ ] DMX4 XLR direkt an WT32 RS485 (kein MAX485!)
- [ ] Alle XLR Stecker richtig verdrahtet (1=GND, 2=-, 3=+)
- [ ] Ethernet Kabel angeschlossen (optional)
- [ ] Status LEDs angeschlossen (optional)

### Nach dem Anschalten:

- [ ] Power LED leuchtet
- [ ] (optional) Status LED ist Grün/Blau
- [ ] Ethernet verbunden (grüne LED am Netzwerk-Port)
- [ ] Serial Monitor zeigt "Setup complete!"

---

## ⚡ MAX485 PINOUT (Standard DIP-8)

```
MAX485 Modul:

       ┌─────────┐
   B →│1      8│← VCC (3.3V)
   A →│2      7│← GND
  RE →│3      6│← A (zu XLR Pin 3)
  DE →│4      5│← B (zu XLR Pin 2)
       └─────────┘

Verbindungen:
- Pin 1 (B) ← WT32 RX
- Pin 2 (A) ← WT32 TX
- Pin 3 (RE) ← WT32 EN (mit Pin 4 verbunden)
- Pin 4 (DE) ← WT32 EN (mit Pin 3 verbunden)
- Pin 5 (B) → XLR Pin 2
- Pin 6 (A) → XLR Pin 3
- Pin 8 (VCC) ← 3.3V
- GND ← WT32 GND

💡 Typischerweise Pin 3 & 4 zusammen auf WT32 EN Pin!
```

---

## 🎯 CODE REFERENZ

```cpp
// DMX1
#define DMX1_TX_PIN 9
#define DMX1_RX_PIN 10
#define DMX1_EN_PIN 27

// DMX2
#define DMX2_TX_PIN 1
#define DMX2_RX_PIN 3
#define DMX2_EN_PIN 25

// DMX3
#define DMX3_TX_PIN 16
#define DMX3_RX_PIN 17
#define DMX3_EN_PIN 26

// DMX4 (integriert!)
#define DMX4_TX_PIN 17
#define DMX4_RX_PIN 5
#define DMX4_EN_PIN 33

// Status LEDs
#define STATUS_LED_RED 36
#define STATUS_LED_GREEN 39
#define STATUS_LED_BLUE 34
```

---

## 🚨 HÄUFIGE FEHLER

❌ **NICHT:** IO17 zweimal nutzen (DMX3 RX + DMX4 TX)  
✅ **JA:** Das ist OK! Auf verschiedenen UARTs.

❌ **NICHT:** IO33 für Status LED  
✅ **JA:** IO33 ist RS485_EN von DMX4!

❌ **NICHT:** 5V und 3.3V gleichzeitig einspeisen  
✅ **JA:** Wähle EINE Stromquelle aus!

❌ **NICHT:** MAX485 für DMX4 kaufen  
✅ **JA:** WT32 hat integrierten RS485!

❌ **NICHT:** XLR Pins falsch verdrahten  
✅ **JA:** Pin 1=GND, Pin 2=Data-, Pin 3=Data+

---

## 📞 Hilfreiche Links

- WT32-ETH01 Datenblatt: In den Uploads gespeichert
- MAX485 Datenblatt: Standard-IC, überall dokumentiert
- ESP32 Pin Reference: https://docs.espressif.com/

---

**Version:** 1.0  
**Datum:** Mai 2026  
**Status:** ✅ Getestet mit Arduino IDE 2.x + ESP32 3.3.8
