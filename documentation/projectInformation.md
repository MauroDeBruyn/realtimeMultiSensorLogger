### **Projectopdracht: "Real-Time Multi-Sensor Logger met PSoC 6 Pioneer Kit en Firebase"**  
**Doel:**  
Ontwerp een **draadloze datalogger** met de **PSoC 6 Pioneer Kit** die **analoge, digitale en I²C-sensordata** verzamelt, tijdstempelt en naar **Firebase** stuurt voor real-time monitoring. Gebruik zoveel mogelijk **interne hardware** van het board (zoals de **Ambient Light Sensor (ALS), Motion Sensor en PDM-microfoon**) om een compact, energiezuinig systeem te bouwen.  

---

### **Projecteisen**  
1. **Hardware**  
   - **PSoC 6 Pioneer Kit** (CY8CKIT-062-WiFi-BT)  


2. **Functionaliteiten**  

   - **Data-acquisitie**:
     - Meet **lichtsterkte** (ALS), **beweging** (IMU), **geluid** (PDM-microfoon).  
     - Lees **externe sensoren** (analoog/digitaal/I²C).  
     
     - **Tijdregistratie**:  
       - Voeg een **real-time klok (RTC)** toe via software of hardware.  
     
     - **Data-export**:  
       - Stuur metingen **periodiek** (bijv. elke 10 sec) naar **Firebase Realtime Database**.  
       - Formatteer data als JSON (bijv. `{timestamp: "2024-05-20T14:30:00", light: 320, motion: 1.2, sound: 65}`).  
     

   - **Lokale logging** (back-up):  
     - Sla data op in het **flash-geheugen** van de PSoC 6 bij netwerkuitval.  


3. **Gebruikersinterface**  

   - **TFT-scherm**: Toon live metingen (licht, beweging, geluid).  

   - **Status-LED’s**:  
     - Groen: Verbonden met Firebase.  
       - Rood: Netwerkfout (gebruik flash-opslag).  


---

### **Stappenplan voor de Student**  
1. **Hardware Setup**  

   - Configureer de **ALS** via de ADC.  
   - Initialiseer de **IMU** via I²C. 
   - Initialiseer **een RTC**. 
   - Stel de **PDM-microfoon** in voor PCM-conversie.  

   - Gebruik **digitale pinnen** voor schakelaars.

   - Stel de **I²C-hardware** in (bijv. BME280).  

#### **2. Software-architectuur**  
1. **Tijdbeheer**:  
   - Gebruik de **PSoC 6 RTC** voor tijdstempels.  
2. **Firebase-integratie**:  
   - Gebruik de **Wi-Fi-module** om HTTP-requests te sturen naar Firebase.  
   - Implementeer **error handling** bij netwerkproblemen (fallback naar flash).  
3. **Lokale UI**:  
   - Toon data op het **TFT-scherm**. 