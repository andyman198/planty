


void doPostMessage( char* httpSERVER, String httpURL, int httpPort, String message)
{
  
  Serial.println("About to send message...");
  // Services/PlantySet?data=AV0001,50,100,150
  if (!client.connect(httpSERVER, httpPort)) {
    Serial.println("No Connection to PostMessage page");
    return;
  }
 
  String httpget = String("GET ") + http_message_url + message + " HTTP/1.1\r\n" +
                   "Host: " + httpSERVER + "\r\n" +
                   "Connection: close\r\n\r\n";
  //Serial.println(httpget);
  client.println(httpget);
  
  Serial.println("MESSAGE SENT: " + httpget);
  Serial.println("MESSAGE SENT: " + message);
  
  
}


void doSendData( char* httpSERVER, String httpURL, int httpPort, int moisture, int temp, int light)
{
  Serial.println("Sampling...sending data");
  // Services/PlantySet?data=AV0001,50,100,150
  if (!client.connect(httpSERVER, httpPort)) {
    // Serial.println("No Connection");
    return;
  }
  String data = String(moisture) + "," +  String(temp) + "," +  String(light);
  String httpget = String("GET ") + http_send_url + data + " HTTP/1.1\r\n" +
                   "Host: " + httpSERVER + "\r\n" +
                   "Connection: close\r\n\r\n";

  client.println(httpget);
  Serial.println("DATA SENT: " + data);
  
  
}

void readHTTPFile( char* httpSERVER, String httpURL, int httpPort)
{
  
    
  // const int httpPort = 80;
  if (!client.connect(httpSERVER, httpPort)) {

    Serial.print("No Connection to ");
    Serial.print(httpSERVER);
    Serial.print(" on port ");
    Serial.println(httpPort);

    return;
  }

  String httpget = String("GET ") + httpURL + " HTTP/1.1\r\n" +
                   "Host: " + httpSERVER + "\r\n" +
                   "Connection: close\r\n\r\n";
  
  client.println(httpget);
  
  ///Serial.println(httpget);
  //client.print("Client available: ");
  //client.println(client.available());




  String file = "";

  while (client.available()) {
    char c = client.read();

    file = file +  c;
    // String line = client.readStringUntil('\r');

    //Serial.print("|" + line + "|");
  }



  // string.substring(from, to)
  // String stringOne = "<HTML><HEAD><BODY>";
  int startPlanty = file.indexOf('^');
  String dataLine = file.substring(startPlanty, file.length());
  file = "";
  // Serial.println("^^^^^|" + dataLine + "|^^^^^");

  // ^0,0,0,0F,F0,FF,00000000,00003000,00060000,0400,AV0001
  // 012345678901234567890123456789012345678901234567890123456789
  //           1         2         3         4         5
  if (dataLine.length() > 0)
  {

    var_onoffbutton =  dataLine.substring(1, 2).toInt();
    var_autoonoffbutton = dataLine.substring(3, 4).toInt();
    var_lightsonoff = dataLine.substring(5, 6).toInt();




    String r = dataLine.substring(7, 9);
    String g = dataLine.substring(10, 12);
    String b = dataLine.substring(13, 15);

    //var_lightscolour_r = string2char(r);
    r.toCharArray(var_lightscolour_r, 3);
    g.toCharArray(var_lightscolour_g, 3);
    b.toCharArray(var_lightscolour_b, 3);

    String st = dataLine.substring(16, 24);
    String pr = dataLine.substring(25, 33);
    String pc = dataLine.substring(34, 42);
    String sm = dataLine.substring(43, 47);

    var_sampleinterval = st.toInt()*1000;
    var_pumpRunTime = pr.toInt()*1000;
    var_pumpCooldownTime = pc.toInt()*1000;
    var_soilMoistureForAutoPump = sm.toInt();

    #if LED_ATTACHED==1
       led_runlights();
    #endif
    // Serial.println("Data Recieved OK!");
 


  } else {
    Serial.println("^^^^^|<GOT NO DATA>|^^^^^");

  }



}

