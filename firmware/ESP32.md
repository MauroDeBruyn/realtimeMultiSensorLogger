# Softwareoverzicht

In dit document worden de gebruikte programmeertalen, libraries, versies en installatie-instructies beschreven.

## 💻 Softwarestack

| Component       | Beschrijving               |
|----------------|----------------------------|
| **Taal**        | Python 3.11                |
| **IDE**         | VSCode + PlatformIO plugin |
| **Firmware**    | Arduino C++ voor ESP32     |
| **Visualisatie**| Python-script via matplotlib |

## 📚 Libraries en afhankelijkheden

### Python (host-side scripts)

Zie `requirements.txt` of installeer met:

```bash
pip install -r requirements.txt
```

Inhoud requirements.txt:

```text
pyserial==3.5
matplotlib==3.7
numpy==1.24
```

### Arduino / PlatformIO (firmware op ESP32)

PlatformIO projectbestand (platformio.ini) specificeert alle dependencies. Voorbeeld:
```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
lib_deps =
    adafruit/Adafruit VL53L0X @ ^1.0.5
    bblanchon/ArduinoJson @ ^6.20.0
```

## 🚀 Installatie-instructies

1. Firmware uploaden
 - Open project in VSCode met PlatformIO.
 - Verbind ESP32 via USB.
 - Klik op “Upload”.
2. Python scripts draaien
 - Installeer dependencies:
 ```bash
  pip install -r requirements.txt
  ```
 - Start script:
 ```bash
 python scripts/visualiseer.py
 ```

## 📌 Opmerkingen
- Zorg dat de juiste poort geselecteerd is in PlatformIO.
- Sommige libraries zijn afhankelijk van specifieke versies (conflicten mogelijk).
- Bij fouten: zie troubleshooting.md.
