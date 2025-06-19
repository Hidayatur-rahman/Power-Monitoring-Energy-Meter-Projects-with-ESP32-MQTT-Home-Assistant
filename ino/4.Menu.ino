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
