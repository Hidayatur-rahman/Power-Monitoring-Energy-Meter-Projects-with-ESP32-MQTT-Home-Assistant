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
