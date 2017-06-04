void sampleSensor()
{
    stype = "Dry";
#if SENSOR_TYPE==0
    Serial.println("Chirp Sampling");
    
    // read
    soilMoistureActual  =  readI2CRegister16bit(0x20, 0);
    tempActual = readI2CRegister16bit(0x20, 5);
    writeI2CRegister8bit(0x20, 3);
    lightActual = readI2CRegister16bit(0x20, 4);
    
    if (soilMoistureActual == -1) {
      stype = "Missing";
    }
    if (soilMoistureActual > 400) {
      stype = "Humid";
    }
    if (soilMoistureActual > 800) {
      stype = "Water";
    }


    
  
#endif
#if SENSOR_TYPE==1
   Serial.println("SHT10 Sampling");
   
   float temp_c;

  float temp_f;

  float humidity;



  // Read values from the sensor

  temp_c = sht1x.readTemperatureC();

  temp_f = sht1x.readTemperatureF();

  humidity = sht1x.readHumidity();

  Serial.print("Temperature: ");

  Serial.print(temp_c, DEC);

  Serial.print("C / ");

  Serial.print(temp_f, DEC);

  Serial.print("F. Humidity: ");

  Serial.print(humidity);

  Serial.println("%");

  
  tempActual =temp_c;
  soilMoistureActual =humidity;

   if (soilMoistureActual <0 ) {
      stype = "Missing";
    }
    if (soilMoistureActual > 40) {
      stype = "Humid";
    }
    if (soilMoistureActual > 80) {
      stype = "Water";
    }

    
#endif
}

  


void storeWiFiInfo(String ssid, String pass)
{
  preferences.begin("planty", false);
  // Remove all preferences under opened namespace
  //preferences.clear();
  // unsigned int counter = preferences.getUInt("counter", 0);

   preferences.putString("wssid", ssid);
   preferences.putString("wpass", pass);
   
  // Close the Preferences
  preferences.end();

}


void outputWifiStatus()
{
  switch (WiFi.status())
        {
          case WL_CONNECTED:
            Serial.println("WL_CONNECTED");
            break;
          case WL_NO_SHIELD:
            Serial.println("WL_NO_SHIELD");
            break;
          case WL_IDLE_STATUS:
            Serial.println("WL_IDLE_STATUS");
            break;
          case WL_NO_SSID_AVAIL:
            Serial.println("WL_NO_SSID_AVAIL");
            break;
          case WL_SCAN_COMPLETED:
            Serial.println("WL_SCAN_COMPLETED");
            break;
          case WL_CONNECT_FAILED:
            Serial.println("WL_CONNECT_FAILED");
            break;
          case WL_CONNECTION_LOST:
            Serial.println("WL_CONNECTION_LOST");
            break;
          case WL_DISCONNECTED:
            // Serial.println("WL_DISCONNECTED");
            Serial.print(".");
            break;
        }
}

void runbuttons()
{
     #if DISPLAY_ATTACHED==1
    
        if (! digitalRead(PIN_BUTTON_A))
        {
          pauseplanty();
        }
        
        if (! digitalRead(PIN_BUTTON_B))
        {      
          Serial.println("** *********************************** **");
          Serial.println("**         DEBUG MODE ACTIVATED        **");
          Serial.println("** *********************************** **");
          
          debug=true;
        }
    
        if (! digitalRead(PIN_BUTTON_C))
        {
            //pumpON = true;
            //pumpTime = var_pumpRunTime+timePassed; // X seconds of pumping
            //bypass = true;
            pump_now();
        }
    
    #endif
}

void unpauseplanty()
{
   #if DISPLAY_ATTACHED==1
      display.clearDisplay();
      display.display();
      display.setCursor(0, 0);
   #endif
      paused = false;
      bypass = false;
      led_flashcolour(0, 255, 0,100, 3);
      led_restorelights();
      Serial.println("** Unpaused **");
}


void pauseplanty()
{
      // Clear Display
      #if DISPLAY_ATTACHED==1
         display.clearDisplay();
         display.display();
         display.setCursor(0, 0);
         display.println("Planty Stopped.");
         display.display(); // actually display all of the above
      #endif
      #if LED_ATTACHED==1
         led_flashcolour(255, 0, 0,100, 3);
         led_godark();
      #endif
      // Pause Planty
      paused = true;
      bypass = true;
      Serial.println("** Paused **");
}


void listNetworks() {
   // scan for nearby networks:
   Serial.println("** Scan Networks **");
   byte numSsid = WiFi.scanNetworks();

   // print the list of networks seen:
   Serial.print("number of available networks:");
   Serial.println(numSsid);

   // print the network number and name for each network found:
   for (int thisNet = 0; thisNet<numSsid; thisNet++) {
     Serial.print(thisNet);
     Serial.print(") ");
     Serial.print(WiFi.SSID(thisNet));
     Serial.print("\tSignal: ");
     Serial.print(WiFi.RSSI(thisNet));
     Serial.print(" dBm");
     Serial.print("\tEncryption: ");
     Serial.println(WiFi.encryptionType(thisNet));
   }
}


void pumpOn()
{
  digitalWrite(PIN_PUMP , HIGH); 
  Serial.print("PUMP ON: ");
}


void pumpOff()
{
  digitalWrite(PIN_PUMP , LOW); 
  if(debug){Serial.println("PUMP OFF: ");}
}


void writeI2CRegister8bit(int addr, int value) {
  Wire.beginTransmission(addr);
  Wire.write(value);
  Wire.endTransmission();
}

unsigned int readI2CRegister16bit(int addr, int reg) {
  Wire.beginTransmission(addr);
  Wire.write(reg);
  Wire.endTransmission();
  delay(100); // was 1100
  Wire.requestFrom(addr, 2);
  unsigned int t = Wire.read() << 8;
  t = t | Wire.read();
  return t;
}


void pause(int seconds)
{
  while(seconds>0) 
  {
    seconds = seconds - 1;
    delay(1000);
  }   
}

void sample()
{

  doSendData(http_host, http_send_url, http_port, soilMoistureActual, tempActual, lightActual);

}



void handleserial()
{
  
  String inputString="";
  
  if (Serial.available()) {
    while (Serial.available()) 
    {
      char inChar = (char)Serial.read();
      inputString += inChar;
      // Serial.println("I: " + inChar);
      // add it to the inputString:
      if (inChar == ';') {
      
       Serial.println("Input: " + inputString);

       if(inputString.startsWith("plantyconfig="))
       {
         //new config data
         // plantyconfig=wifissid,mynewpassword;
         inputString.replace("plantyconfig=","");
         
         String s = getValue(inputString, '|', 0);
         String p = getValue(inputString, '|', 1);
         
         p = p.substring(0,p.length()-1);
        
         Serial.println("NewSSID: " + s);
         Serial.println("NewPWD : " + p);


         storeWiFiInfo( s, p );

          
         //savedSSID = (char*)s;
         //savedPassword = (char*)p;

         // ssid = (char*)s;
         // password = (char*)p;
         
       }
       
       inputString="";
     }
    }
  }
}



//void serialEvent() {
//   while (Serial.available()) {
//     // get the new byte:
//     char inChar = (char)Serial.read();
//     // add it to the inputString:
//     inputString += inChar;
//     Serial.println("Input: " + inputString);
//     // if the incoming character is a newline, set a flag
//     // so the main loop can do something about it:
//     if (inChar == '\n') {
//       stringComplete = true;
//     }
//   }
//}

void pump_now()
{
    #if LED_ATTACHED==1
       led_godark();
    #endif
    
    String message = "Starting pump for " + String(var_pumpRunTime) + " seconds";
    Serial.println(message);
    doPostMessage(http_host, http_message_url, http_port, message);

    unsigned long runtime = var_pumpRunTime;
    unsigned long timenow = millis();

    digitalWrite(PIN_PUMP, HIGH);   // sets the Pump on

    while(  (timenow+runtime) > millis() )
    {
      // do nothing
      delay(100);
      digitalWrite(PIN_PUMP, HIGH);   // sets the Pump on
      led_randomone();
      Serial.print("/\\");
    }
    digitalWrite(PIN_PUMP, LOW);    // sets the Pump off
    Serial.println(" ");

    message = "Pump turned off. Pump Ran for " + String(millis()-timenow) + " milliseconds";
    Serial.println(message);
    doPostMessage(http_host, http_message_url, http_port, message);

    
    pumpDelay = var_pumpCooldownTime;
    led_runlights();
}


void run_debug()
{
   // only send data every minute
    String debug = "";
    
    debug += "\n | Seconds: " + String(seconds) + "/" + String(var_sampleinterval/1000);
    debug += "\n | Soil = " + String(soilMoistureActual);
   
    debug += "\n | Soil Type = " + stype;
    debug += "\n | Pump Run Time:" + String(var_pumpRunTime);
    debug += "\n | Pump Cooldown:" + String(var_pumpCooldownTime);
    debug += "\n | AutoPumpOn:" + String(var_autoonoffbutton);
    debug += "\n | PumpOn:" + String(pumpON);
    debug += "\n | SampleTime:" + String(var_sampleinterval);
    debug += "\n | Temp:" + String(tempActual);
    debug += "\n | Light:" + String(lightActual);
    debug += "\n | A:" + String(digitalRead(PIN_BUTTON_A));
    debug += "\n | B:" + String(digitalRead(PIN_BUTTON_B));
    debug += "\n | C:" + String(digitalRead(PIN_BUTTON_C));

    debug += "\n | ON OFF:         " + String(var_onoffbutton);
    debug += "\n | AUTO ON OFF:    " + String(var_autoonoffbutton);
      
    debug += "\n | LIGHT ON OFF:   " + String(var_lightsonoff);
    debug += " LIGHT R:        "; Serial.println(var_lightscolour_r);
    debug += " LIGHT G:        "; Serial.println(var_lightscolour_g);
    debug += " LIGHT B:        "; Serial.println(var_lightscolour_b);
      
    debug += "\n | SAMPLE INTERVAL:" + String(var_sampleinterval);
    debug += "\n | PUMP RUN TIME:  " + String(var_pumpRunTime);
    debug += "\n | PUMP COOLDOWN:  " + String(var_pumpCooldownTime);
    debug += "\n | SOIL MOISTURE:  " + String(var_soilMoistureForAutoPump);
 

    
    Serial.println(debug);
}

String getValue(String data, char separator, int index)
{
 int found = 0;
  int strIndex[] = {0, -1  };
  int maxIndex = data.length()-1;
  for(int i=0; i<=maxIndex && found<=index; i++){
  if(data.charAt(i)==separator || i==maxIndex){
  found++;
  strIndex[0] = strIndex[1]+1;
  strIndex[1] = (i == maxIndex) ? i+1 : i;
  }
 }
  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}


char* string2char(String command){
    if(command.length()!=0){
        char *p = const_cast<char*>(command.c_str());
        return p;
    }
}

 


