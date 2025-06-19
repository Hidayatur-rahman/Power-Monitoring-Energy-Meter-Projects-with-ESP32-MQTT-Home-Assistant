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

void getData() {
  currentMillis_AmbilData = millis();
  if (currentMillis_AmbilData - previousMillis_AmbilData >= interval_AmbilData)
  {
    previousMillis_AmbilData = currentMillis_AmbilData;  
    // Serial.print("Custom Address:");
    // Serial.println(pzem.readAddress(), HEX);
    // Read the data from the sensor
     voltage            = pzem.voltage();
     current            = pzem.current();
     power              = pzem.power();
     energykWh          = pzem.energy();
     frequency          = pzem.frequency();
     pf                 = pzem.pf();
  }
}
void calculateEnergy(){
     apparentPower      = voltage*current;
     capacitorCalculate = ((((current)*sqrt(1-(((power)/(voltage*current))*((power)/(voltage*current)))))/(voltage*314))*1000000);
     reactivePower      = sqrt(apparentPower * apparentPower - power * power);
     phaseAngle         = acos(pf);
     phaseAngleDegrees  = phaseAngle * 180.0 / PI;
    //  sinPhi          = sqrt(1 - pf * pf);
     energyWh           = energykWh*1000;
     voltageInt         = voltage;
}

// void resetAndSaveEnergy(){
//   currentMillis_ResetEnergyDay = millis();
//   if (currentMillis_ResetEnergyDay - previousMillis_ResetEnergyDay >= interval_ResetEnergyDay)
//   {
//     previousMillis_ResetEnergyDay = currentMillis_ResetEnergyDay;

//      if (timeClient.getHours() == 23 && timeClient.getMinutes() == 59) {
//       dailyEnergy = energykWh;
//       monthlyEnergy += dailyEnergy;

//       String payload2 = "{\"dailyEnergy\":" + String(dailyEnergy) + "}";

//      client.publish(mqttTopic2, payload2.c_str());
//      pzem.resetEnergy();
//     }
//  }
//   currentMillis_ResetEnergyMonth = millis();
//   if (currentMillis_ResetEnergyMonth- previousMillis_ResetEnergyMonth >= interval_ResetEnergyMonth)
//   {
//     previousMillis_ResetEnergyMonth = currentMillis_ResetEnergyMonth;
//      // Check if it's the first day of the month and midnight (00:00)
//      if (timeClient.getDay() == 1 && timeClient.getHours() == 0 && timeClient.getMinutes() == 0) {
//       previousMonthlyEnergy = monthlyEnergy; 
//       pzem.resetEnergy();
//     }
//  }
// }

void setupWiFi(){
  if (enableWiFi==1){
  WiFi.setHostname(hostname);
   WiFi.begin(ssid);
  // Blynk.config(BLYNK_AUTH_TOKEN);
  // Configure OTA
  ArduinoOTA.setHostname("ESP32_EnergyMonitoring");  // Change to your desired hostname
  // ArduinoOTA.setPassword("");    // Set a password for security (optional)
  ArduinoOTA.onStart([]() {
    Serial.println("OTA Update Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nOTA Update End");
  });
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
  // mqtt 
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
    // Serial.println("Connecting to MQTT...");
    if (client.connect("ESP32Client")) {
      // Serial.println("Connected to MQTT");
      client.subscribe(mqttTopic2);
    } else {
      // Serial.print("Failed, rc=");
      // Serial.print(client.state());
      // Serial.println(" Retrying in 5 seconds");
      delay(5000);
    }
  }
}

// Function to handle incoming MQTT messages
void callback(char* topic, byte* payload, unsigned int length) {
  // Serial.print("Message arrived on topic: ");
  // Serial.print(topic);
  // Serial.print(". Message: ");
  String message;

  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  // Serial.println(message);

  // Parse JSON data
  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, message);

  if (error) {
    // Serial.print("deserializeJson() failed: ");
    // Serial.println(error.c_str());
    return;
  }

  if (String(topic) == mqttTopic2) {
        dailyEnergy = doc["daily_energy"];
        monthlyEnergy = doc["monthly_energy"];
        // Serial.print("Daily Energy: ");
        // Serial.println(dailyEnergy);
        // Serial.print("Monthly Energy: ");
        // Serial.println(monthlyEnergy);
    }
}
//loop wifi
void Wireless_Telementry(){
 if(enableWiFi==1){
 timeClient.update();
//Blynk.run(); 
 ArduinoOTA.handle();
 if (!client.connected()){
 reconnect();
}
 client.loop();
 
//send data to mqtt
currentMillis_KirimData = millis();
if (currentMillis_KirimData - previousMillis_KirimData >= interval_KirimData)
 {
  previousMillis_KirimData = currentMillis_KirimData;

  int intPower = static_cast<int>(power);
  int intApparentPower = static_cast<int>(apparentPower);

  // Blynk.virtualWrite(V1, voltage);
  // Blynk.virtualWrite(V2, current);
  // Blynk.virtualWrite(V3, power);
  // Blynk.virtualWrite(V4, energykWh);
  // Blynk.virtualWrite(V5, frequency);
  // Blynk.virtualWrite(V6, pf);
  // Blynk.virtualWrite(V7, apparentPower);
  // Blynk.virtualWrite(V8, previousDailyEnergy);
  // Blynk.virtualWrite(V9, capacitorCalculate);

  String payload1 = "{\"voltage\":" + String(voltage) +
                   ",\"current\":" + String(current) +
                   ",\"power\":" + String(intPower) +
                   ",\"energykWh\":" + String(energykWh) +
                   ",\"frequency\":" + String(frequency) +
                   ",\"pf\":" + String(pf) +
                   ",\"capacitorCalculate\":" + String(capacitorCalculate) +
                   ",\"apparentPower\":" + String(intApparentPower) + "}";

  client.publish(mqttTopic1, payload1.c_str());
  }
 }
}

void menu(){
  lcd.createChar(VOLT_ICON, volt_icon);
  lcd.createChar(COS_ICON, cos_icon);
  lcd.createChar(ARROW, arrow_icon);
  lcd.createChar(ARROW2, arrow_icon2);
  lcd.createChar(ARROW3, arrow_icon3);
  lcd.createChar(DOT, dot_icon);
  //Konfigurasi Tombol 
  statusBtnDwn  = digitalRead(btnDwn);
  statusBtnOk   = digitalRead(btnOk);
  statusBtnBack = digitalRead(btnBack);

  //Untuk button Dwn
  if (DWN && halaman == 1) {
    DWN = false;
    menuItem ++;
    if (menuItem >3 )menuItem = 1;
  }
  //Untuk button ok
  if (OKE) {
    OKE = false;
    if (halaman == 1 && menuItem == 1) {
      halaman = 2;
    }
     else if (halaman == 1 && menuItem == 2) {
      halaman = 3;
    }
     else if (halaman == 1 && menuItem == 3){
      halaman = 4;
     } 
  }
  //Untuk button back
  if (BACK) {
    BACK = false;
    if (halaman == 2 || halaman == 3 || halaman == 4) {
        halaman = 1;
    }
  }
 if (halaman == 1) {
    lcd.setCursor(0, 0);
    lcd.write(ARROW);
    lcd.print(" POWER MONITORING ");
    lcd.write(ARROW2);

    if (menuItem == 1) {
      lcd.setCursor(0, 3);
      lcd.write(ARROW3);
      lcd.print(" POWER");
    } else {
      lcd.setCursor(0, 3);
      lcd.print("  POWER");
    }
    if (menuItem == 2) {
      lcd.setCursor(0, 1);
      lcd.write(ARROW3);
      lcd.print(" PARAMETER");
    } else {
      lcd.setCursor(0, 1);
      lcd.print("  PARAMETER");
    }
    if (menuItem == 3){
      lcd.setCursor(0, 2);
      lcd.write(ARROW3);
      lcd.print(" ENERGY");
    }else{
      lcd.setCursor(0, 2);
      lcd.print("  ENERGY");
    }
  }
 else if (halaman == 2){
    lcd.setCursor(0, 0);
    lcd.write(ARROW);
    lcd.print(" POWER MONITORING ");
    lcd.write(ARROW2);

    lcd.setCursor(0, 1);
    lcd.write(VOLT_ICON);
    lcd.print(" ");
    lcd.setCursor(2, 1);
    lcd.print(voltage, 0);
    lcd.print(" V");

    lcd.setCursor(0, 2);
    lcd.print("A ");
    // lcd.write(DOT);
    lcd.print(current, 1);
    padding10(current);
    lcd.print("A");

    lcd.setCursor(0, 3);
    lcd.print("P ");
    // lcd.write(DOT);
    lcd.print(power, 0);
    lcd.print(" W");
    padding100(power);

    lcd.setCursor(11, 1);
    lcd.write(COS_ICON);
    lcd.print(" ");
    lcd.setCursor(13, 1);
    lcd.print(pf);

    lcd.setCursor(11, 2);
    lcd.print("f ");
    // lcd.write(DOT);
    lcd.setCursor(13, 2);
    lcd.print(frequency, 0);
    lcd.setCursor(17, 2);
    lcd.print("Hz");

    lcd.setCursor(11, 3);
    lcd.print("E ");
    lcd.setCursor(13, 3);
    lcd.print(dailyEnergy, 0);
    lcd.setCursor(17, 3);
    lcd.print("kWh");
 }
  else if (halaman == 3){
  lcd.setCursor(0, 0);
  lcd.write(ARROW);
  lcd.print(" POWER MONITORING ");
  lcd.write(ARROW2);
  lcd.setCursor(0, 1);
  lcd.print("P ");
  // lcd.write(DOT);
  lcd.print(power, 0);
  lcd.print(" W");
  padding100(power);
  lcd.setCursor(0, 3);
  lcd.print("Q ");
  // lcd.write(DOT);
  lcd.print(reactivePower, 1);
  lcd.print(" Var");
  padding100(reactivePower);
  lcd.setCursor(0, 2);
  lcd.print("S ");
  // lcd.write(DOT);
  lcd.print(apparentPower, 0);
  lcd.print(" Va");
  padding100(apparentPower);
  lcd.setCursor(12, 1);
  lcd.write(COS_ICON);
  lcd.print(" ");
  lcd.print(pf, 1);
  lcd.setCursor(12, 2);
  lcd.print("C ");
  // lcd.write(DOT);
  lcd.print(capacitorCalculate, 0);
  lcd.print(" uF");
  lcd.setCursor(12, 3);
  lcd.print("I ");
  lcd.print(current, 1);
  padding10(current);
  lcd.print("A");
  // lcd.setCursor(12, 3);
  // lcd.print("E:"); 
  // lcd.print()
  }
  else if (halaman == 4){
    lcd.setCursor(0, 0);
    lcd.write(ARROW);
    lcd.print("    ENERGY USE    ");
    lcd.write(ARROW2);
    lcd.setCursor(0, 1);
    lcd.print("Today : ");
    // lcd.write(DOT);
    lcd.print(dailyEnergy, 0);
    lcd.print(" kWh");
    padding100(dailyEnergy);
    lcd.setCursor(0, 2);
    lcd.print("Month : ");
    // lcd.write(DOT);
    lcd.print(monthlyEnergy, 0);
    lcd.print(" kWh");
    padding100(monthlyEnergy);
    lcd.setCursor(0, 3);
    lcd.print("Total : ");
    // lcd.write(DOT);
    lcd.print(energykWh, 0);
    lcd.print(" kWh");
    padding100(energykWh);
    // lcd.setCursor(0, 3);
    // lcd.print("LastMonth");
    // lcd.write(DOT);
    // lcd.print(previousMonthlyEnergy);
    // lcd.print(" kWh");
    // padding10(previousMonthlyEnergy);

  }
}
//=================================== KONFIGURASI TOMBOL =======================================//
void settingButton() {
  if (statusBtnDwn != statusAkhirBtnDwn) {
    if (statusBtnDwn == 0) {
      DWN = true;
    }
  }
  statusAkhirBtnDwn = statusBtnDwn;

  if (statusBtnOk != statusAkhirBtnOk) {
    if (statusBtnOk == 0) {
      OKE = true;
    }
    lcd.clear();
  }
  statusAkhirBtnOk = statusBtnOk;

  if (statusBtnBack != statusAkhirBtnBack) {
    if (statusBtnBack == 0) {
      BACK = true;
    }
    lcd.clear();
  }
  statusAkhirBtnBack = statusBtnBack;
}
 //=============================================================================================//
 void padding100(int padVar){
  if (padVar<10){
    lcd.print("   ");
  }
  else if (padVar<100){
    lcd.print("  ");
  }
  else if (padVar<1000){
    lcd.print(" ");
  }
}
void padding10(int padVar){
  if(padVar<10){
    lcd.print(" ");
  }
}

// void serialMonitor(){
//    currentMillis_Serial = millis();
//   if (currentMillis_Serial- previousMillis_Serial >= interval_Serial)
//   {

//     previousMillis_Serial = currentMillis_Serial;
// //Check if the data is valid
//     if(isnan(voltage)){
//         Serial.println("Error reading voltage");
//     } else if (isnan(current)) {
//         Serial.println("Error reading current");
//     } else if (isnan(power)) {
//         Serial.println("Error reading power");
//     } else if (isnan(energykWh)) {
//         Serial.println("Error reading energy");
//     } else if (isnan(frequency)) {
//         Serial.println("Error reading frequency");
//     } else if (isnan(pf)) {
//         Serial.println("Error reading power factor");
//     } else {

//         // Print the values to the Serial console
//         Serial.print("Voltage        : ");       Serial.print(voltage);               Serial.println(" V");
//         Serial.print("Current        : ");       Serial.print(current);               Serial.println(" A");
//         Serial.print("Power          : ");       Serial.print(power);                 Serial.println(" W");
//         Serial.print("Apparent Power : ");       Serial.print(apparentPower);         Serial.println(" Va");
//         Serial.print("Reactive Power : ");       Serial.print(reactivePower, 2);      Serial.println(" Var");
//         Serial.print("Energy         : ");       Serial.print(energykWh, 3);          Serial.println(" kWh");
//         Serial.print("Frequency      : ");       Serial.print(frequency, 1);          Serial.println(" Hz");
//         Serial.print("Power Factor   : ");       Serial.println(pf);
//         Serial.print("Cap Calculate  : ");       Serial.print(capacitorCalculate);    Serial.println(" uF");
//         Serial.print("Phase Angle    : ");       Serial.println(phaseAngleDegrees, 2);

//         if (phaseAngle > 0) {
//         Serial.println("Phase          : Leading Power Factor   ");
//        }else if (phaseAngle < 0) { 
//         Serial.println("Phase          : Lagging Power Factor   ");
//        }else {
//         Serial.println("Phase          : Unity (No phase shift) ");
//       } 
//         Serial.print("Analod Read    : ");       Serial.println(Temp);
      
//     }    
//  }
// }

   
