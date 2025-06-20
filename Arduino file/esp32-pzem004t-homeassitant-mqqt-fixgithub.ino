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
const char *mqttTopic1 = "home/sensor/allpower";
const char *mqttTopic2 = "home/sensor/energy";

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