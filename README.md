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
1. Real-Time Energy Monitoring. Measures voltage (V), current (A), power (W), energy (kWh), frequency (Hz), and power factor with the PZEM-004T sensor.
2. Wireless Data Transmission. Wifi and MQQT : Esp32 sends data to Home Assistant without cables.
3. Energy Accumlated Function. Accumulated energy (kWh), Useful for monthly billing tracking.
   
## **Hardware Requirements** <a name="hardware-requirements"></a>  
1. Hardware Component
   - ESP32 for data processing and WiFi connectivity.
   - Pzem004t to measure: Voltage (V), Current (A), Power (W), Energy (kWh), Frequency (Hz), Power Factor.
   - Lcd 20x4 for data display
   - Push Button for select menu lcd display
   - Wire
   - Box
   - Terminal Block
   - Power Supply 220 AC to 5V Dc for Arduino and Pzem
   - PCB
2. Hardware Configuration
   - Schematics
     
      ![image](https://github.com/user-attachments/assets/406f03c4-97b0-487b-ae78-fd535504a460)
     
   - Wiring 
     
      <img src="https://github.com/user-attachments/assets/8f447a4b-0fe4-4c89-9615-2d9e7c3dad05" width="915" alt="Untitled Design">

   - Implementaion
     
        
## **Firmware Setup** <a name="firmware-setup"></a>

## Arduino Code
Disini saya menggunakan esp32 karena esp32 memiliki dual core cpu yang dimana core 1 akan menghandel program dan yang core 2 untuk wifi saja, ini sangat baik agar wifi tidak mengganggu kinerja program yang berjalan. selain itu esp32 dilengkapi dengan wifi yang dimana digunakan untuk mengirim data ke homeassitant melalui mqqt. berikut program untuk power dan energy monitoring:

##
```cpp
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
LiquidCrystal_I2C lcd(0x27, 20, 4); 
#include <esp_system.h>

char ssid[] = "Firda Kost";
char hostname[] = "Energy_Metering";   

#define btnDwn  26
#define btnOk   27
#define btnBack 25
TaskHandle_t Core2;                              //SYSTEM PARAMETER  - Used for the ESP32 dual core operation
const char *mqttServer = "10.10.9.74";
const int mqttPort = 1883;
const char *mqttTopic1 = "home/sensor/allpower";
const char *mqttTopic2 = "home/sensor/energy";
const char* ntpServer        = "pool.ntp.org"; 
const int gmtOffset_sec      = 8*3600;          // Set GMT+8 time zone offset (8 hours)
const int daylightOffset_sec = 0;
WiFiUDP ntpUDP;
NTPClient timeClient (ntpUDP, ntpServer, gmtOffset_sec);
WiFiClient espClient;
PubSubClient client(espClient);
//
bool signupOK              = false;
boolean statusBtnDwn       = false;
boolean statusBtnOk        = false;
boolean statusBtnBack      = false;
boolean statusAkhirBtnDwn  = false;
boolean statusAkhirBtnOk   = false;
boolean statusAkhirBtnBack = false;
boolean DWN                = false;
boolean OKE                = false;
boolean BACK               = false;
bool
enableWiFi     = 1,
WIFI           = 0;
int
halaman        = 1,
menuItem       = 1,
resetEnergy,
voltageInt,
Temp;
float 
frequency,
voltage,
current, 
power, 
energykWh,
energyWh,
sinPhi,
reactivePower,
apparentPower,
phaseAngle,
phaseAngleDegrees,
pf,
capacitorCalculate;
float 
dailyEnergy,
previousDailyEnergy,
monthlyEnergy,
previousMonthlyEnergy;

unsigned long
sendDataPrevMillis             = 0,
currentMillis_KirimData        = 0,
last_time_KirimData            = 0,
current_time_KirimData         = 0,
previousMillis_KirimData       = 0,

currentMillis_AmbilData        = 0,
last_time_AmbilData            = 0,
current_time_AmbilData         = 0,
previousMillis_AmbilData       = 0,

currentMillis_Serial           = 0,
last_time_Serial               = 0,
current_time_Serial            = 0,
previousMillis_Serial          = 0,

currentMillis_ResetEnergyDay   = 0,
last_time_ResetEnergyDay       = 0,
current_time_ResetEnergyDay    = 0,
previousMillis_ResetEnergyDay  = 0,

currentMillis_ResetEnergyMonth = 0,
last_time_ResetEnergyMonth     = 0,
current_time_ResetEnergyMonth  = 0,
previousMillis_ResetEnergyMonth= 0;

const long
interval_KirimData        = 3000,
interval_AmbilData        = 1000,
interval_Serial           = 2000,
interval_ResetEnergyDay   = 30000,
interval_ResetEnergyMonth = 30000;

#if defined(ESP32)
PZEM004Tv30 pzem(Serial2, 33, 32);
#else
PZEM004Tv30 pzem(Serial2);
#endif
#define VOLT_ICON 5
byte volt_icon[8] =
{
  B00011,
  B00110,
  B01100,
  B11111,
  B00011,
  B00110,
  B01100,
  B11000
};
#define COS_ICON 6
byte cos_icon[8] =
{
  B01000,
  B10010,
  B10101,
  B10101,
  B01110,
  B00100,
  B00100,
  B00100
};
#define ARROW 7
byte arrow_icon[8] =
{
  B00011,
  B00110,
  B01100,
  B11000,
  B11000,
  B01100,
  B00110,
  B00011
};
#define ARROW2 8
byte arrow_icon2[8] =
{
  B11000,
  B01100,
  B00110,
  B00011,
  B00011,
  B00110,
  B01100,
  B11000
};
#define ARROW3 9
byte arrow_icon3[8] =
{
  B00100,
  B00100,
  B00110,
  B11111,
  B11111,
  B00110,
  B00100,
  B00100
};
#define DOT 10
byte dot_icon[8] =
{
  B00000,
  B00000,
  B00100,
  B00000,
  B00000,
  B00000,
  B00100,
  B00000
};

// BLYNK_WRITE(V0){
// resetEnergy = param.asInt();
// if(resetEnergy == 1){
//   pzem.resetEnergy();
//  }
// }

void coreTwo(void * pzemParameters){
  setupWiFi();
  while(1){
  Wireless_Telementry();
  }
}
void setup() {
  delay(200);
  xTaskCreatePinnedToCore(coreTwo,"coreTwo",10000,NULL,0,&Core2,0);   //ENABLE DUAL CORE MULTITASKING
  Serial.begin(115200);
  pinMode(btnDwn,  INPUT_PULLUP);
  pinMode(btnOk,   INPUT_PULLUP);
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
void loop(){
  menu();
  settingButton();
  getData();
  calculateEnergy();
  // resetAndSaveEnergy();
  // serialMonitor();
}
   
