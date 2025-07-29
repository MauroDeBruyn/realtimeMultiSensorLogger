> [!Tip]
>
> Plaats hier **schema's, PCB's en BoM's**.
>
> Structuur: `schematics/`, `pcb/`, `bom/`, `3d/`.
>
> Noem gebruikte tools (bijv. KiCad, Eagle) in een README binnen deze map.


# Hardwareoverzicht

Dit document bevat een overzicht van de gebruikte hardwarecomponenten in dit project. Alle datasheets zijn lokaal opgeslagen in de map `/resources/datasheets/`.

## 🧩 Componentenlijst

| Componentnaam  | Functie             | Fabrikant       | Datasheet                                  | More info                          | Supplier                          |
|----------------|---------------------|------------------|---------------------------------------------|-----------------------------------|-----------------------------------|
| VL53L0X        | Afstandssensor      | STMicroelectronics | [VL53L0X.pdf](/resources/datasheets)     | [wiki.st.com](https://www.st.com/VL53L0X) | [mouser](mouser.be)                          |
| NEMA 17        | Stappenmotor        | --               | [NEMA17.pdf](/resources/datasheets)       | --                                | [mouser](mouser.be)                          |
| ESP32 DevKit   | Microcontroller     | Espressif        | [ESP32.pdf](/resources/datasheets)         | [docs.espressif.com](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/index.html#introduction) | [mouser](mouser.be)                          |

## 🔌 Aansluitschema

> Voeg hier een afbeelding of diagram toe waarin de bedrading en verbindingen van de componenten worden weergegeven.

- Bestand: `./schematics/wiringschema.png`
- Alternatief: teken met [Fritzing](https://fritzing.org) en exporteer als PNG/SVG.

## 📏 Specificaties en toleranties

- Voedingsspanning: 5V via USB
- Motorstroom: max. 1.2A
- Sensorbereik: 20 mm – 2000 mm (VL53L0X)
- Communicatie: I²C (3.3V-logica)

## 📦 Reserve-onderdelen en versies

- Kalibratie uitgevoerd met firmwareversie `v1.0.3`.
- ESP32 DevKit V4 (let op: oudere versies hebben andere pinout).
