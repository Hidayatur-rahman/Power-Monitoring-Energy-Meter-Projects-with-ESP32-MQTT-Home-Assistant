# Power-Monitoring-Energy-Meter-Projects-with-ESP32-MQTT-Home-Assistant
Author : Hidayatur rahman

## **Table of Contents**  
1. [Introduction](#introduction)
2. [Features](#features)
3. [Hardware Requirements](#hardware-requirements)  
4. [Firmware Setup](#firmware-setup)  
5. [Home Assistant Integration](#home-assistant-integration)  
6. [Calibration](#calibration)  
7. [Troubleshooting](#troubleshooting)  
8. [License](#license)
   
## **Introduction** <a name="introduction"></a>  
This my first documented Arduino IoT project is both exciting and challenging. I'll be building A Smart Power meter using an ESP32, Pzem for data collection, MQTT for communication, and Home Assistant for visualization. This project will help understand core IoT concepts like sensor data collection, wireless connectivity, and real-time monitoring.

## **Features** <a name="features"></a> 
---
| Feature              | Description                                             |
|---------------------|---------------------------------------------------------|
| Real-Time Monitoring| Reads data every second                                 |
| Dual-Core Operation | Core 0 handles UI; Core 1 handles OTA/MQTT/NTP          |
| MQTT Publish        | Sends JSON-formatted data every 3 seconds               |
| MQTT Subscribe      | Receives daily/monthly energy updates                   |
| LCD Menu Interface  | Interactive UI using buttons and display                |
| Power Factor Calc   | Includes apparent/reactive power & capacitor estimate   |
| OTA Update Support  | Remote firmware updates over WiFi                       |
| NTP Time Sync       | Time sync using pool.ntp.org                            |

---

## 📘 Project Description

This project measures and monitors AC electrical parameters like:

- Voltage (V)
- Current (A)
- Active Power (W)
- Apparent Power (VA)
- Reactive Power (VAR)
- Power Factor (CosΦ)
- Frequency (Hz)
- Energy Usage (kWh)

It displays the values on a **20x4 LCD** and sends data via **MQTT**. Three buttons allow users to navigate an on-device menu. OTA and NTP functionality are also included.

---
## 🔩 Hardware Components

| Component          | Description                                  |
|-------------------|----------------------------------------------|
| ESP32 Dev Board   | Dual-core MCU with WiFi                      |
| PZEM004Tv30       | Power monitoring module (AC only)            |
| 20x4 I2C LCD      | Data display module                          |
| 3x Push Buttons   | Menu navigation (DOWN, OK, BACK)             |
| Power Source      | 5V USB or regulator to power ESP32           |
| Capacitor (calc.) | Displayed as required for PF correction      |
---

## 🔌 Hardware Connections

### ESP32 Wiring

| ESP32 GPIO | Function          | Connection              |
|------------|-------------------|--------------------------|
| GPIO 33    | UART RX2 (PZEM Tx)| PZEM004T Tx             |
| GPIO 32    | UART TX2 (PZEM Rx)| PZEM004T Rx             |
| GPIO 25    | BACK Button       | Tactile switch to GND   |
| GPIO 26    | DOWN Button       | Tactile switch to GND   |
| GPIO 27    | OK Button         | Tactile switch to GND   |
| GPIO 21/22 | I2C SDA/SCL       | LCD I2C module          |

 - Schematics
     
      ![image](https://github.com/user-attachments/assets/406f03c4-97b0-487b-ae78-fd535504a460)
     
 - Wiring 
     
      <img src="https://github.com/user-attachments/assets/8f447a4b-0fe4-4c89-9615-2d9e7c3dad05" width="915" alt="Untitled Design">

 - Implementaion
     
---
## **Firmware Setup** <a name="firmware-setup"></a>

## 🧠 Software Architecture

- **Main Loop (Core 0)**
  - Handles button input
  - Updates LCD menu
  - Reads data from sensor
  - Performs calculations

- **Core 1 Task**
  - Handles OTA server
  - Maintains MQTT connection
  - Publishes JSON telemetry
  - Syncs time using NTP
    
---
## 🖥️ Menu System

The LCD interface provides an intuitive navigation system:

### Main Menu

1. Power Monitoring
2. Electrical Parameters
3. Energy Usage

### Page Details

#### 1. Power Monitoring
- Voltage
- Current
- Power (Active)
- Power Factor
- Frequency
- Daily Energy

#### 2. Electrical Parameters
- P (Active Power)
- Q (Reactive Power)
- S (Apparent Power)
- CosΦ
- Capacitor (µF)
- Current

#### 3. Energy Usage
- Daily kWh
- Monthly kWh
- Total Energy (from PZEM)

---

## 🧮 Electrical Calculations

| Parameter           | Formula                                               |
|---------------------|--------------------------------------------------------|
| Apparent Power (S)  | S = V × I                                              |
| Reactive Power (Q)  | Q = √(S² - P²)                                         |
| Phase Angle         | θ = acos(PF)                                           |
| Capacitor Value (µF)| C = ((I × √(1 - PF²)) / (V × 2πf)) × 10⁶               |

---

## 📡 MQTT Integration

### Published Topic: `home/sensor/allpower`
Every 3 seconds, payload:
```json
{
  "voltage": 220.1,
  "current": 1.23,
  "power": 271,
  "energykWh": 2.12,
  "frequency": 50.0,
  "pf": 0.95,
  "capacitorCalculate": 7.3,
  "apparentPower": 280.0
}
```

### Subscribed Topic: `home/sensor/energy`
Expected payload:
```json
{
  "daily_energy": 1.25,
  "monthly_energy": 34.6
}
```

---

## 🔧 OTA Updates

OTA is automatically enabled when connected to WiFi.
To update:
1. Open Arduino IDE
2. Select `ESP32_EnergyMonitoring` port (network)
3. Upload sketch via OTA

Progress is shown via Serial Monitor.

---

## Arduino Code
I'm using the ESP32 because it features a dual-core CPU, where:
- Core 1 handles the main program execution
- Core 2 is dedicated solely to WiFi operations
  
This architecture is ideal for preventing WiFi-related tasks from interfering with the core program's performance. Additionally, the ESP32 comes with built-in WiFi capabilities, which I utilize to transmit data to Home Assistant via MQTT.

##
```cpp
/* ===================== LIBRARIES & CONSTANTS ===================== */
#include <WiFi.h>
#include <WiFiClient.h>
#include <NTPClient.h>
#include <ArduinoOTA.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "math.h"
#include <PZEM004Tv30.h>
#include <LiquidCrystal_I2C.h>
#include <esp_system.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

/* ===================== NETWORK CONFIGURATION ===================== */
char ssid[] = "Firda Kost";
char hostname[] = "Energy_Metering";
const char *mqttServer = "10.10.9.74";
const int mqttPort = 1883;
const char *mqttTopic1 = "home/sensor/allpower";      // Mqqt topic to Publish 
const char *mqttTopic2 = "home/sensor/energy";        // Mqqt topic to Get Data Daily Energy and Monthly Energy from Homeassitant

/* ====================== TIME CONFIGURATION ======================= */
const char* ntpServer = "pool.ntp.org"; 
const int gmtOffset_sec = 8*3600;
const int daylightOffset_sec = 0;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntpServer, gmtOffset_sec);

/* ===================== HARDWARE CONFIGURATION ==================== */
#define btnDwn 26
#define btnOk 27
#define btnBack 25
#if defined(ESP32)
PZEM004Tv30 pzem(Serial2, 33, 32);
#else
PZEM004Tv30 pzem(Serial2);
#endif

/* ======================= GLOBAL VARIABLES ======================== */
TaskHandle_t Core2;
bool enableWiFi = 1, WIFI = 0;
bool statusBtnDwn = false, statusBtnOk = false, statusBtnBack = false;
bool statusAkhirBtnDwn = false, statusAkhirBtnOk = false, statusAkhirBtnBack = false;
bool DWN = false, OKE = false, BACK = false;
int halaman = 1, menuItem = 1, resetEnergy, voltageInt, Temp;
float frequency, voltage, current, power, energykWh, energyWh, sinPhi, reactivePower;
float apparentPower, phaseAngle, phaseAngleDegrees, pf, capacitorCalculate;
float dailyEnergy, previousDailyEnergy, monthlyEnergy, previousMonthlyEnergy;

unsigned long previousMillis_KirimData = 0, previousMillis_AmbilData = 0;
const long interval_KirimData = 3000, interval_AmbilData = 1000;

/* ===================== LCD CUSTOM CHARACTERS ===================== */
#define VOLT_ICON 5
byte volt_icon[8]   = {B00011,B00110,B01100,B11111,B00011,B00110,B01100,B11000};
#define COS_ICON 6
byte cos_icon[8]    = {B01000,B10010,B10101,B10101,B01110,B00100,B00100,B00100};
#define ARROW 7
byte arrow_icon[8]  = {B00011,B00110,B01100,B11000,B11000,B01100,B00110,B00011};
#define ARROW2 8
byte arrow_icon2[8] = {B11000,B01100,B00110,B00011,B00011,B00110,B01100,B11000};
#define ARROW3 9
byte arrow_icon3[8] = {B00100,B00100,B00110,B11111,B11111,B00110,B00100,B00100};
#define DOT 10
byte dot_icon[8]    = {B00000,B00000,B00100,B00000,B00000,B00000,B00100,B00000};

/* ======================== CORE FUNCTIONS ========================== */
void coreTwo(void * pzemParameters) {
  setupWiFi();
  while(1) { Wireless_Telementry(); }
}

void setup() {
  delay(200);
  xTaskCreatePinnedToCore(coreTwo, "coreTwo", 10000, NULL, 0, &Core2, 0);
  
  Serial.begin(115200);
  pinMode(btnDwn, INPUT_PULLUP);
  pinMode(btnOk, INPUT_PULLUP);
  pinMode(btnBack, INPUT_PULLUP);
  pinMode(36, INPUT);

  lcd.begin();
  lcd.backlight();
  lcd.clear();
  delay(1000);
  lcd.setCursor(0, 1);
  lcd.write(ARROW);
  lcd.print(" POWER MONITORING ");
  lcd.write(ARROW2);
}

void loop() {
  menu();
  settingButton();
  getData();
  calculateEnergy();
}

/* ===================== MEASUREMENT FUNCTIONS ====================== */
void getData() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis_AmbilData >= interval_AmbilData) {
    previousMillis_AmbilData = currentMillis;  
    voltage = pzem.voltage();
    current = pzem.current();
    power = pzem.power();
    energykWh = pzem.energy();
    frequency = pzem.frequency();
    pf = pzem.pf();
  }
}

void calculateEnergy() {
  apparentPower = voltage * current;
  capacitorCalculate = ((current * sqrt(1 - pow((power)/(voltage*current), 2))) / (voltage * 314)) * 1000000;
  reactivePower = sqrt(apparentPower * apparentPower - power * power);
  phaseAngle = acos(pf);
  phaseAngleDegrees = phaseAngle * 180.0 / PI;
  energyWh = energykWh * 1000;
  voltageInt = voltage;
}

/* ======================= NETWORK FUNCTIONS ======================= */
WiFiClient espClient;
PubSubClient client(espClient);

void setupWiFi() {
  if (enableWiFi) {
    WiFi.setHostname(hostname);
    WiFi.begin(ssid);
    
    ArduinoOTA.setHostname("ESP32_EnergyMonitoring");
    ArduinoOTA.onStart([]() { Serial.println("OTA Update Start"); });
    ArduinoOTA.onEnd([]() { Serial.println("\nOTA Update End"); });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("OTA Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
      Serial.printf("OTA Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });
    ArduinoOTA.begin();
    
    client.setServer(mqttServer, mqttPort);
    client.setCallback(callback);
    client.subscribe(mqttTopic2);
    timeClient.begin();
    timeClient.update();
    WIFI = 1;
  }
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("ESP32Client")) {
      client.subscribe(mqttTopic2);
    } else {
      delay(5000);
    }
  }
}

/* ========= Get Data Daily and Monthly Energy from Homassistant ========== */

void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (int i = 0; i < length; i++) message += (char)payload[i];
  
  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, message);
  
  if (!error && String(topic) == mqttTopic2) {
    dailyEnergy = doc["daily_energy"];
    monthlyEnergy = doc["monthly_energy"];
  }
}

/* ====================== PUBLISH DATA FROM PZEM   ======================= */

void Wireless_Telementry() {
  if(enableWiFi) {
    timeClient.update();
    ArduinoOTA.handle();
    if (!client.connected()) reconnect();
    client.loop();
    
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis_KirimData >= interval_KirimData) {
      previousMillis_KirimData = currentMillis;
      
      String payload = "{\"voltage\":" + String(voltage) +
                     ",\"current\":" + String(current) +
                     ",\"power\":" + String(power) +
                     ",\"energykWh\":" + String(energykWh) +
                     ",\"frequency\":" + String(frequency) +
                     ",\"pf\":" + String(pf) +
                     ",\"capacitorCalculate\":" + String(capacitorCalculate) +
                     ",\"apparentPower\":" + String(apparentPower) + "}";
      
      client.publish(mqttTopic1, payload.c_str());
    }
  }
}

/* ===================== LCD & MENU FUNCTIONS ===================== */
void menu() {
  lcd.createChar(VOLT_ICON, volt_icon);
  lcd.createChar(COS_ICON, cos_icon);
  lcd.createChar(ARROW, arrow_icon);
  lcd.createChar(ARROW2, arrow_icon2);
  lcd.createChar(ARROW3, arrow_icon3);
  lcd.createChar(DOT, dot_icon);
  
  statusBtnDwn = digitalRead(btnDwn);
  statusBtnOk = digitalRead(btnOk);
  statusBtnBack = digitalRead(btnBack);

  if (DWN && halaman == 1) {
    DWN = false;
    menuItem++;
    if (menuItem > 3) menuItem = 1;
  }
  
  if (OKE) {
    OKE = false;
    if (halaman == 1 && menuItem == 1) halaman = 2;
    else if (halaman == 1 && menuItem == 2) halaman = 3;
    else if (halaman == 1 && menuItem == 3) halaman = 4;
    lcd.clear();
  }
  
  if (BACK) {
    BACK = false;
    if (halaman > 1) halaman = 1;
    lcd.clear();
  }

  switch(halaman) {
    case 1:
      lcd.setCursor(0, 0);
      lcd.write(ARROW);
      lcd.print(" POWER MONITORING ");
      lcd.write(ARROW2);
      
      lcd.setCursor(0, menuItem == 2 ? 1 : 1);
      lcd.print(menuItem == 2 ? ">" : " ");
      lcd.print(" PARAMETER");
      
      lcd.setCursor(0, menuItem == 3 ? 2 : 2);
      lcd.print(menuItem == 3 ? ">" : " ");
      lcd.print(" ENERGY");
      
      lcd.setCursor(0, menuItem == 1 ? 3 : 3);
      lcd.print(menuItem == 1 ? ">" : " ");
      lcd.print(" POWER");
      break;
      
    case 2:
      displayPowerPage();
      break;
      
    case 3:
      displayParameterPage();
      break;
      
    case 4:
      displayEnergyPage();
      break;
  }
}

void displayPowerPage() {
  lcd.setCursor(0, 0);
  lcd.write(ARROW);
  lcd.print(" POWER MONITORING ");
  lcd.write(ARROW2);

  lcd.setCursor(0, 1);
  lcd.write(VOLT_ICON);
  lcd.print(" ");
  lcd.print(voltage, 0);
  lcd.print(" V");

  lcd.setCursor(0, 2);
  lcd.print("A ");
  lcd.print(current, 1);
  padding10(current);
  lcd.print("A");

  lcd.setCursor(0, 3);
  lcd.print("P ");
  lcd.print(power, 0);
  lcd.print(" W");
  padding100(power);

  lcd.setCursor(11, 1);
  lcd.write(COS_ICON);
  lcd.print(" ");
  lcd.print(pf);

  lcd.setCursor(11, 2);
  lcd.print("f ");
  lcd.print(frequency, 0);
  lcd.print(" Hz");

  lcd.setCursor(11, 3);
  lcd.print("E ");
  lcd.print(dailyEnergy, 0);
  lcd.print(" kWh");
}

void displayParameterPage() {
  lcd.setCursor(0, 0);
  lcd.write(ARROW);
  lcd.print(" POWER MONITORING ");
  lcd.write(ARROW2);
  
  lcd.setCursor(0, 1);
  lcd.print("P ");
  lcd.print(power, 0);
  lcd.print(" W");
  padding100(power);
  
  lcd.setCursor(0, 2);
  lcd.print("S ");
  lcd.print(apparentPower, 0);
  lcd.print(" Va");
  padding100(apparentPower);
  
  lcd.setCursor(0, 3);
  lcd.print("Q ");
  lcd.print(reactivePower, 1);
  lcd.print(" Var");
  padding100(reactivePower);
  
  lcd.setCursor(12, 1);
  lcd.write(COS_ICON);
  lcd.print(" ");
  lcd.print(pf, 1);
  
  lcd.setCursor(12, 2);
  lcd.print("C ");
  lcd.print(capacitorCalculate, 0);
  lcd.print(" uF");
  
  lcd.setCursor(12, 3);
  lcd.print("I ");
  lcd.print(current, 1);
  padding10(current);
  lcd.print("A");
}

void displayEnergyPage() {
  lcd.setCursor(0, 0);
  lcd.write(ARROW);
  lcd.print("    ENERGY USE    ");
  lcd.write(ARROW2);
  
  lcd.setCursor(0, 1);
  lcd.print("Today: ");
  lcd.print(dailyEnergy, 0);
  lcd.print(" kWh");
  padding100(dailyEnergy);
  
  lcd.setCursor(0, 2);
  lcd.print("Month: ");
  lcd.print(monthlyEnergy, 0);
  lcd.print(" kWh");
  padding100(monthlyEnergy);
  
  lcd.setCursor(0, 3);
  lcd.print("Total: ");
  lcd.print(energykWh, 0);
  lcd.print(" kWh");
  padding100(energykWh);
}

/* ===================== UTILITY FUNCTIONS ===================== */
void settingButton() {
  if (statusBtnDwn != statusAkhirBtnDwn) {
    if (statusBtnDwn == 0) DWN = true;
    statusAkhirBtnDwn = statusBtnDwn;
  }
  
  if (statusBtnOk != statusAkhirBtnOk) {
    if (statusBtnOk == 0) OKE = true;
    statusAkhirBtnOk = statusBtnOk;
  }
  
  if (statusBtnBack != statusAkhirBtnBack) {
    if (statusBtnBack == 0) BACK = true;
    statusAkhirBtnBack = statusBtnBack;
  }
}

void padding100(int padVar) {
  if (padVar < 10) lcd.print("   ");
  else if (padVar < 100) lcd.print("  ");
  else if (padVar < 1000) lcd.print(" ");
}

void padding10(int padVar) {
  if (padVar < 10) lcd.print(" ");
}
```
