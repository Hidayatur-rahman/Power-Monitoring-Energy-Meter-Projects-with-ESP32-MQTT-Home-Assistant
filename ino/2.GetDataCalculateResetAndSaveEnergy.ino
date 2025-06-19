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
