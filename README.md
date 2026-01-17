# ESP32 Quiz-Buzzer Sender (Lolin 32 Lite)

Dies ist die Software für die **Buzzer-Einheiten** (Sender). Die Buzzer sind darauf optimiert, monatelang mit einer einzigen Akkuladung auszukommen. Sie erwachen nur beim Drücken aus dem Tiefschlaf, senden ein Signal und schlafen sofort wieder ein.

Das vollständige Tutorial mit Gehäusedaten und Bauanleitung findest du auf [prilchen.de](https://prilchen.de/esp32-projekt-quiz-buzzer-wer-drueckt-als-erster)

![Projektbild](https://prilchen.de/wp-content/uploads/2026/01/image-6.png)

## 🚀 Highlights
- **EXT0-Wakeup:** Der ESP32 verbraucht im Tiefschlaf fast keine Energie und wacht blitzschnell per Tastendruck auf.
- **ESP-NOW:** Nutzt das schnellste Funkprotokoll für ESP32-Module (minimale Latenz).
- **Batterieoptimiert:** - Keine unnötigen WiFi-Scans.
  - Deaktivierbare serielle Ausgabe (`#define DEBUG 0`).
  - Automatische Sperre gegen Mehrfach-Trigger beim Gedrückthalten.

## 🛠 Hardware-Anforderungen
- **Mikrocontroller:** ESP32 Lolin 32 Lite.
- **Akku:** Kleiner LiPo-Akku (empfohlen: 250 mAh).
- **Taster:** 1x Taster (angeschlossen an **GPIO 4** gegen GND).
- **Gehäuse:** Kompaktes 3D-Druck-Gehäuse (Dateien auf Maker World verlinkt).
- **Schrauben:** 4x M2 selbstschneidend (10 mm) zum Verschließen.

## 💻 Einrichtung & Programmierung
1. Öffne das Projekt in **Visual Studio Code** mit **PlatformIO**.
2. **WICHTIG:** Bevor du den Sketch hochlädst, musst du die **MAC-Adresse der Zentrale** (den Würfel) im Code eintragen (Variable `broadcastAddress`).
3. Setze im Sketch für jeden Buzzer eine eindeutige `playerID` (oder nutze die MAC-Erkennung der Zentrale).
4. Flashe den ESP32 über das USB-Kabel.

### platformio.ini
Nutze diese Konfiguration für einen stabilen Build:
```ini
[env:lolin32_lite]
platform = [https://github.com/pioarduino/platform-espressif32/releases/download/stable/platform-espressif32.zip](https://github.com/pioarduino/platform-espressif32/releases/download/stable/platform-espressif32.zip)
board = lolin32_lite
framework = arduino
monitor_speed = 115200
