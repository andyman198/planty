/*
    ##############################################
    ## Customer Setup
    ## Edit the values below
    ##############################################
*/

// type of sensor
// 0 = chirp sensor  - https://www.adafruit.com/product/1298
// 1 = SHT sensor    - https://www.adafruit.com/product/1965

#define SENSOR_TYPE 1
#define DISPLAY_ATTACHED 0
#define LED_ATTACHED 0

bool debug = false; // debug mode;

   #if SENSOR_TYPE==1 && DISPLAY_ATTACHED==1
      #error("Cant use display with SHT10 Sensor!");
   #endif





#include <WiFi.h>
#include <Preferences.h>
#include <SPI.h>
#include <Wire.h>

#if SENSOR_TYPE == 1
  #include <SHT1x.h>
#endif

#if LED_ATTACHED==1
   #include <Adafruit_NeoPixel.h>
#endif

#if DISPLAY_ATTACHED==1
   #include <Adafruit_GFX.h>
   #include <Adafruit_SSD1306.h>
#endif


Preferences preferences;

// Wifi username and password

//#define WLAN_SSID_PRI       "NemesisNet"
//#define WLAN_PASS_PRI       "Mjolnir117"
//#define WLAN_SSID_SEC       "RTI-Lab-RIF"
//#define WLAN_PASS_SEC       "3305447742"

char* wifi_ssid_custom;
char* wifi_pass_custom;
bool wifi_custom = false;

//#define WLAN_SSID       "NemesisNet"
//#define WLAN_PASS       "Mjolnir117"

String planty_id = "";


// const char* http_host = "192.168.17.240";
// const int http_port = 1709;

char* http_host = "planty.andy-net.com";
int http_port = 80;



String http_recv_url;
String http_send_url;
String http_message_url;


//char* ssid = "CortanasBetrayal";
//char* password = "ithurtswhenip";

//char* ssid = "NETGEAR99";
//char* password = "silkypond875";

//char* ssid = "NemesisNet";
//char* password = "Mjolnir117";

// Instructions

/*   https://learn.adafruit.com/adafruit-feather-huzzah-esp8266/pinouts
     ########################################
     ### Connect Solenoid
     ########################################
        Solenoid signal pin to P12 (default)
        Solenoin COM to Arduino GND

     ########################################
     ### Connect Sensor
     ########################################
        sensor     1   2   X
                   3   4   5


        1 = Arduino Pin 2
        2 = SCL
        3 = GND
        4 = SDA
        5 = 3V

     ########################################
     ### Connect Pump
     ########################################
     Pump Voltage to 3v / USB / Batt
     Pump Ground to Solenoid NC
*/

/*
    ##############################################
    ## END Customer Setup
    ##############################################
*/



/*
    ########################################
    ### Pin Setup
    ########################################
*/


// PIN 15 - Empty

// HUZZAH 16
// #define PIN_BUTTON_A 0   // A button
// #define PIN_BUTTON_B 16  // B Button
// #define PIN_BUTTON_C 2  // C button  / should be 2
// #define PIN_CHIRP 14      // Chirp Sensor  // should be 14
// #define PIN_PUMP 12      // Solenoid
// #define PIN_LED  13      // Ring Light

// HUZZAH 32
#define PIN_BUTTON_A 15   // A button
#define PIN_BUTTON_B 32  // B Button
#define PIN_BUTTON_C 14  // C button  / should be 2

#define dataPin  23
#define clockPin 22



#if (SENSOR_TYPE==1)
  
  SHT1x sht1x(dataPin,clockPin);
  
#endif

#define PIN_CHIRP 13      // Chirp Sensor  // should be 14
#define PIN_PUMP 12      // Solenoid
#define PIN_LED  27      // Ring Light

#if LED_ATTACHED==1
   Adafruit_NeoPixel plantyring = Adafruit_NeoPixel(24, PIN_LED, NEO_GRB + NEO_KHZ800);  
   #define LEDColor = "#00FF00";
  bool ledon = false;
  int ledr = 0;
  int ledg = 0;
  int ledb = 0;
  
  int fadespeed = 5;
  
  int fade_red = 0;
  int fade_green = 0;
  int fade_blue = 0;
  
  int fade_redGoal = 0;
  int fade_greenGoal = 255;
  int fade_blueGoal = 0;
  
  int fade_counter = 0;
  
  bool fadeon = false;

#endif


/*
    Wiring Setup

    Connection
    Chirp pin 1 - no connection
    Chirp pin 2 - Arduino VCC
    Chirp pin 3 - Arduino A5
    Chirp pin 4 - Arduino A4
    Chirp pin 5 - Arduino pin 2
    Chirp pin 6 - Arduino GND

*/



// Serial communications
// String inputString = "";         // a string to hold incoming data


bool pumpON = false;
// bool autoPumpON = false;

/************************* WebServices Setup *********************************/

int var_onoffbutton = 0;
int var_autoonoffbutton = 0;
int var_lightsonoff = 0;

char var_lightscolour_r[3] = "00";
char var_lightscolour_g[3] = "00";
char var_lightscolour_b[3] = "00";

String var_led_r = "0";
String var_led_g = "0";
String var_led_b = "0";

unsigned long   var_sampleinterval = 3600*1000;
unsigned long   var_pumpRunTime = 0;
unsigned long   var_pumpCooldownTime = 60*1000;
int  var_soilMoistureForAutoPump = 1000;

long lastConnectionTime = 0;


WiFiClient client;


// ******** Read Vars ********
String onoffbutton;
String autoonoffbutton;
String sampletimefeed;
String lightsonoff;
String lightscolour;


// int timePump = 10;  // Time to pump when soil moisture goes low (in secs)
// int timeWait = 60;  // After Pumping, time to wait before next measurement (in secs)

int pumpDelay = 1000 * 60; // wait at least a minute before doing any pumping
int pumpTime = 0;
// int soilMoistureForAutoPump = 400;
// int pumpRunTime = 3000; // _ 2 secs to process = a ten second pump

#if DISPLAY_ATTACHED==1
   Adafruit_SSD1306 display = Adafruit_SSD1306();
#endif
String stype = "Dry";




float soilMoistureActual = 0;     // variable to store the read value
// int soilMoistureHighLow = 0;     // variable to store the read value

float tempActual = 0;     // variable to store the read value
int lightActual = 0;     // variable to store the read value

// time in seconds between uploads of soil moisture (for charting)
int sampleTime = 60;
 // 60 * 15; // 15 mins

#if DISPLAY_ATTACHED
   #if (SSD1306_LCDHEIGHT != 32)
      #error("Height incorrect, please fix Adafruit_SSD1306.h!");
   #endif
#endif

uint32_t x = 0;

unsigned long currentTime  = 0; //this variable will be overwritten by millis() each iteration of loop
unsigned long pastTime     = 0; //no time has passed yet
unsigned long timePassed   = 0;
int           currentState = 0; //the default state
unsigned long seconds = 0;

// int displayAttached = 1;

bool bypass = false;
bool paused = false;

void setup() {

 
    
  // make sure pump isnt running first thing



  // initialize serial - has to be first!
  Serial.begin(38400);
  delay(2000);

//    if (! digitalRead(PIN_BUTTON_B))
//    { // didnt detect OLED display ... dont do any displaying
//      Serial.println("OLED Not Detected");
//      displayAttached = 0;
//      
//    }else{
//       Serial.println("OLED Detected");
//    }
//    
   
 
   
  Serial.println("");
  Serial.println("");


#if LED_ATTACHED==1


  Serial.println("Pulse!");
  plantyring.setBrightness(255);
  plantyring.begin();
  plantyring.show(); // Initialize all pixels to 'off'
  colorpulse();
#endif

  // serial comms
  // inputString.reserve(200);


// ########### START LCD Setup

  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  #if DISPLAY_ATTACHED==1
 
    //display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
    // Clear the buffer.
    display.clearDisplay();
    display.display();
    

  
      Serial.println("Display was detected ... setting up");
      display.clearDisplay();
      display.display();
    
      // text display tests
      display.setTextSize(1);
      display.setTextColor(WHITE);
    
      display.clearDisplay();
      display.setCursor(0, 0);
      Serial.println("Planty Hello");
      display.display();
      yield();
      // delay (1000);
  

   #endif 

 
  // ########### START WIFI Setup

  Serial.println("Planty V0.15");


  
  led_flashcolour(0, 0, 100, 200, 2);


  WiFi.disconnect();
  

 
  int conLED = 0;

  
  int s=0;
  int conn = 0;
  Serial.println(" "); 
 

  
  while ( conn ==0 )
  {

        
        handleserial();
        
        // Open Preferences with my-app namespace. Each application module, library, etc.
        // has to use namespace name to prevent key name collisions. We will open storage in
        // RW-mode (second parameter has to be false).
        // Note: Namespace name is limited to 15 chars
        preferences.begin("planty", false);
        // Remove all preferences under opened namespace
        //preferences.clear();
        // unsigned int counter = preferences.getUInt("counter", 0);
      
        Serial.println(" ");
        String ssid = preferences.getString("wssid", "");
        String pass = preferences.getString("wpass", "");
        
        // Close the Preferences
        preferences.end();
         
        //Serial.println("s-wssid: " + ssid);
        //Serial.println("s-wpass: " + pass);
      
        if(ssid!="")
        {
          wifi_custom = true;
          
          unsigned char* ssidvalChar = (unsigned char*) ssid.c_str(); // cast from string to unsigned char*
          unsigned char* passvalChar = (unsigned char*) pass.c_str(); // cast from string to unsigned char*
      
          
           char* storedSSID = (char*) ssidvalChar;
           // (char*) ssid.c_str(); 
          // string2char(ssid);
           char* storedPass =  (char*)passvalChar;
           // (char*) pass.c_str(); 
          // string2char(pass);
          
         // Serial.print("c-wssid: ");
          //Serial.println(storedSSID);
          //Serial.print("c-wpass: ");
          //Serial.println(storedPass);
      
          wifi_ssid_custom = storedSSID;
          wifi_pass_custom = storedPass;
        }else{
            delay(2000);
        }


       // connect to custom
       if(wifi_custom == true)
       {
         
          WiFi.disconnect();
          WiFi.begin(wifi_ssid_custom, wifi_pass_custom);
          delay (3000);
          
          Serial.print("Connecting to ");
          Serial.print(wifi_ssid_custom);
          Serial.print("/");
          Serial.println(wifi_pass_custom);
              
             
          Serial.print("With MAC ");
          Serial.println(WiFi.macAddress());
          s=0;
          // ten seconds to connect to primary wifi
          while (   (WiFi.status() != WL_CONNECTED) && s<10) {
                Serial.println("Connecting Custom " + String(s) + "/10");
                s+=1;
                #if LED_ATTACHED==1
                   led_godark();
          
                   plantyring.setPixelColor(conLED, 0, 0, 255);
                   plantyring.show();
                #endif
                conLED += 1;
          
                if (conLED >= 24) {
                  conLED = 0;
                }
            
            
                outputWifiStatus();
            
                delay(2000);
          }
        
          // did we get connected to primary?
          if(WiFi.status() == WL_CONNECTED)
          {
             conn=3;
             break;
          }
       }

       
  }
  
 if(WiFi.SSID()=="NemesisNet")
 { 
     http_host = "192.168.17.240";
     http_port = 1709;
     Serial.print("Connecting Local - NemesisNet On Prem");
 }




  // set up web services using mac address
  String mac = String(WiFi.macAddress());
  mac.replace(":","-");
  planty_id = mac;
  http_recv_url = "/services/PlantyGet?plantyid=" + String(planty_id);
  http_send_url = "/services/PlantySet?data="+ String(planty_id) + ",";
  http_message_url =  "/services/PlantyPostMessage?data="+ String(planty_id) + ",";
  
  // ########### END WIFI Setup

  // ########### START CHIRP Setup
  #if SENSOR_TYPE==0
    Serial.println("Chirp Setup");
    // ########### START CHIRP Setup
    pinMode(PIN_CHIRP, OUTPUT);
    digitalWrite(PIN_CHIRP, LOW); //Reset the Chirp
    //delay(1); maybe allow some time for chirp to reset
    digitalWrite(PIN_CHIRP, HIGH); //Go out from reset
    writeI2CRegister8bit(0x20, 3); //send something on the I2C bus
    delay(2000); //allow chirp to boot
    // ########### END CHIRP Setup
  #endif
  #if SENSOR_TYPE==1
    Serial.println("SHT10 Setup");
     // ########### START SHT10 Setup
     
     // ########### END   SHT10 Setup
  #endif
  

  #if LED_ATTACHED==1
     led_godark();
     // green flash
     led_flashcolour(100, 255, 100, 200, 5);
     led_godark();
  #endif
  
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  
  // init done
  Serial.println("OLED begin");

  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  #if DISPLAY_ATTACHED==1
    display.display();
  #endif




  // Serial.println("IO Setup");

  pinMode(PIN_BUTTON_A, INPUT_PULLUP);
  pinMode(PIN_BUTTON_B, INPUT_PULLUP);
  pinMode(PIN_BUTTON_C, INPUT_PULLUP);
  pinMode(PIN_PUMP, OUTPUT);


  if (! digitalRead(PIN_BUTTON_A))
  {
    Serial.println("Debug Mode");
    debug = true;
  }
 



  
  // ########### START Web Service Setup
  doPostMessage(http_host, http_message_url, http_port, "Starting-up");
   // ########### END Web Service Setup
 
  Serial.println("End Setup");



}



void loop() {


  handleserial();

  
  pumpOff();


  readHTTPFile(http_host, http_recv_url, http_port);



  // always handle time past
  pastTime    = currentTime; //currentTime at this point is the current time from the previous iteration, this should now be pastTime
  currentTime = millis();    //currentTime is now the current time (again).
  timePassed = currentTime - pastTime; //this is roll-over proof, if currentTime is small, and pastTime large, the result rolls over to a small positive value, the time that has passed
  // Serial.println("Pump Time:" + String(pumpTime) + " timepassed: " + timePassed + " pastTime: " + pastTime + " currenttime:" + currentTime);

  seconds = seconds + (timePassed);

  if (bypass == false)
  {

#if LED_ATTACHED==1



    if (fadeon == true)
    {


      if (fade_red > fade_redGoal) {
        fade_red = fade_red - fadespeed;
      }
      else if (fade_red < fade_redGoal) {
        fade_red = fade_red + fadespeed;
      }

      if (fade_green > fade_greenGoal) {
        fade_green = fade_green - fadespeed;
      }
      else if (fade_green < fade_greenGoal) {
        fade_green = fade_green + fadespeed;
      }

      if (fade_blue > fade_blueGoal) {
        fade_blue = fade_blue - fadespeed;
      }
      else if (fade_blue < fade_blueGoal) {
        fade_blue = fade_blue + fadespeed;
      }

      if (fade_red == fade_redGoal && fade_green == fade_greenGoal && fade_blue == fade_blueGoal) {
        // Serial.println("Reached Color " + String(fade_counter));

        if (fade_counter > 6) {
          fade_counter = 0;
        }
        if (fade_counter == 0) {
          //red
          fade_redGoal = 255;
          fade_greenGoal = 0;
          fade_blueGoal = 0;
        }
        else if (fade_counter == 1) {
          //green
          fade_redGoal = 0;
          fade_greenGoal = 255;
          fade_blueGoal = 0;
        }
        else if (fade_counter == 2) {
          //blue
          fade_redGoal = 0;
          fade_greenGoal = 0;
          fade_blueGoal = 255;
        }
        else if (fade_counter == 3) {
          //white
          fade_redGoal = 255;
          fade_greenGoal = 255;
          fade_blueGoal = 255;
        }
        else if (fade_counter == 4) {
          //magenta
          fade_redGoal = 255;
          fade_greenGoal = 0;
          fade_blueGoal = 255;
        }
        else if (fade_counter == 5) {
          //yellow
          fade_redGoal = 255;
          fade_greenGoal = 255;
          fade_blueGoal = 0;
        }
        else if (fade_counter == 6) {
          //cyan
          fade_redGoal = 0;
          fade_greenGoal = 255;
          fade_blueGoal = 255;
        }
        fade_counter++;
      }


      led_setcolour(fade_red, fade_green, fade_blue); 
      // Serial.println("fading");
      // Serial.println(String(fade_red) + "," + String(fade_green) + "," + String(fade_blue));
    }

#endif


    sampleSensor();

 
    

    // soilMoistureHighLow  =  digitalRead(soilDigitalPin);

   
   

    if (pumpDelay > 0)
    {
      // Serial.println("Pump Delay:" + String(pumpDelay));
      pumpDelay = pumpDelay - timePassed;
    }


    // dry check
    if(debug==true){
       Serial.println("Pump Delay: " + String(pumpDelay));
       Serial.println("Pump Moisture: " + String(soilMoistureActual));
       Serial.println("Pump Kick in at: " + String(var_soilMoistureForAutoPump));
    }



    // Water Plant
    if (pumpDelay <= 0)
    {
      // is it lower than 300? is so, activate autopump if enabled
      if (soilMoistureActual < var_soilMoistureForAutoPump && var_autoonoffbutton == true && pumpTime <= 0 && soilMoistureActual > -1)
      {
        // Serial.println("Moisture too low - activating pump");
        //*plantyActionFeed.publish("AUTOPUMP");
        // pumpON = true;
        //digitalWrite(PUMP, HIGH);   // sets the Pump on
        // pumpTime = var_pumpRunTime + timePassed; // X seconds of pumping
        //rainbow the lights
        // led_randomall();
        pump_now();
        // bypass = true;
      }
    }


    runbuttons();


    if (debug == true)
    {
      run_debug();
    }

  

    // Serial.println(String(seconds) + "/" + String(var_sampleinterval));
    int milliseconds =var_sampleinterval-seconds ;
    int vhours = (int) milliseconds / (1000 * 60 * 60);
    int vminutes =  (int) (milliseconds % (1000*60*60)) / (1000*60);
    int vseconds = (int)  ((milliseconds % (1000*60*60)) % (1000*60)) / 1000;
    Serial.println(String(vhours) + " hours, " + String(vminutes) + " minutes and " + String(vseconds) + " seconds until next sample");

    

    
    if (seconds >= var_sampleinterval)
    {
      seconds = 0;
     
      sample();
    }

  } else {
    // bypass mode
    // pump check

    // check to see if we are on a timed pump
    if (pumpTime > 0)
    {
      // rainbow effect
      led_randomone();
      pumpTime = pumpTime - timePassed;

      if (pumpTime <= 0)
      {
        // clear rainbow effect

        led_restorelights();

        Serial.println("Pump time passed = turning off ");
        pumpON = false;
        // digitalWrite(PUMP, LOW);    // sets the Pump off
        pumpDelay = var_pumpCooldownTime; // wait 5 minutes;
        bypass = false;
      }
    }
    if ( timePassed >= 1000 );           // increasing variable "seconds" every second
    {
      seconds = seconds + (int)timePassed / 1000;
      pastTime = currentTime;
    }



  }


  #if DISPLAY_ATTACHED==1
      // Display
      // Has to be last to avoid flashes
      display.clearDisplay();
      display.display();
      display.setCursor(0, 0);
      display.println("###### Planty ######");
      display.println("(\\__/) Moisture = " + String(soilMoistureActual));
      display.println("(O.o ) Soil = " + stype);
      display.println("(> < ) Pump = " + String(digitalRead(PIN_PUMP)));
    
      yield();
      display.display();
  #endif

  delay(5000);


}







