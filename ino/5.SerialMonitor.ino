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