void led_runlights()
{
  #if LED_ATTACHED==1
 

    if (var_lightsonoff == 0)
    {
      ledon = false; 
      fadeon = false;
      led_godark();
    } 
    if (var_lightsonoff == 1)
    {
      ledon = true; 
      fadeon = false;
      //String color = r + g + b;
      String color =String(var_lightscolour_r) + String(var_lightscolour_g)+ String(var_lightscolour_b);

    

    led_setcolourfromstring(color);

    } 
    if (var_lightsonoff == 2)
    {
      fadeon = true;
      fadespeed = 5;
    }
    if (var_lightsonoff == 3)
    {
      fadeon = true;
      fadespeed = 51;
    }  
#endif


}

void led_flashcolour(int red, int green, int blue, int interval, int times)
{
  #if LED_ATTACHED==1

  plantyring.setBrightness(255);
  for (int t = 0; t < times; t++)
  {
    for (int led = 0; led < 25; led++)
    {
      plantyring.setPixelColor(led, red, green, blue );
    }
    plantyring.show();
    delay(interval);
    led_godark();
    plantyring.show();
    delay(interval);

  } 
#endif
}

void led_randomall()
{
  #if LED_ATTACHED==1


  // set all LEDs to random brightness and colour
  for (int led = 0; led < 25; led++)
  {

    int r = random(0, 255);
    int g = random(0, 255);
    int b = random(0, 255);
    int brt = (r + g + b) / 3;

    // dont have an unlit one
    if (brt < 50)
    {
      brt = 50;
    }

    plantyring.setPixelColor(led, r, g, b );
    plantyring.setBrightness(brt);

  }


  plantyring.show();
  
  #endif
}

void led_randomone()
{
  #if LED_ATTACHED==1
   

  int led = random(0, 25);
  int r = random(17, 255);
  int g = random(17, 255);
  int b = random(17, 255);
  int brt = (r + g + b) / 3;

  // dont have an unlit one
  //if (brt < 50)
  //{
  //  brt = 50;
  //}

  plantyring.setPixelColor(led, r, g, b );
  plantyring.setBrightness(brt);
  plantyring.show();
  #endif
}

void led_setcolourfromstring(String color)
{
  #if LED_ATTACHED==1
  

  long number = (int) strtol( &color[0], NULL, 16);

  // Split them up into r, g, b values
  int r = number >> 16;
  int g = number >> 8 & 0xFF;
  int b = number & 0xFF;

  led_setcolour(r, g, b);
  #endif
}

void led_restorelights()
{
  #if LED_ATTACHED==1
   

  // clear previous effect
  led_godark();
  //restore light if it was on
  if (ledon = true)
  {
    led_setcolour(ledr, ledg, ledb);
    int highcol = ledr;
    if (ledg > highcol) {
      highcol = ledg;
    }
    if (ledb > highcol) {
      highcol = ledb;
    }
    plantyring.setBrightness(highcol);
  }
  #endif
}

void led_setcolour(int red, int green, int blue)
{
  #if LED_ATTACHED==1


//  Serial.println("");
//  Serial.print("Setting Colour: ");
//  Serial.print(red);
//  Serial.print(",");
//  Serial.print(green);
//  Serial.print(",");
//  Serial.println(blue);
//  Serial.println("");
  // get most vibrant colour to define brightness
  int highcol = red;
  
  if (green > highcol) {
    highcol = green;
  }
  if (blue > highcol) {
    highcol = blue;
  }


  // set pixel colours and brightness
  for (int led = 0; led < 25; led++)
  {
    plantyring.setPixelColor(led, red, green, blue );
    plantyring.setBrightness(highcol);

  }
  plantyring.show();
  #endif
}

void led_godark()
{
  #if LED_ATTACHED==1
 

  for (int led = 0; led < 25; led++)
  {
    plantyring.setPixelColor(led, 0, 0, 0 );
  }
  plantyring.show();
#endif
}

void colorpulse()
{
#if LED_ATTACHED==1


  plantyring.setBrightness(255);
  // int led=0;
  // int color=0;

  // for each color
  for (int color = 0; color < 3; color++)
  {
    for (int led = 0; led < 25; led++)
    {

      int lightlevel = (255 / 24) * led;
      if (lightlevel == 0)
      {
        lightlevel = 5;
      }
      if (color == 0)
      {
        plantyring.setPixelColor(led, lightlevel, 0, 0 );
      }
      if (color == 1)
      {
        plantyring.setPixelColor(led, 0, lightlevel, 0 );
      }
      if (color == 2)
      {
        plantyring.setPixelColor(led, 0, 0, lightlevel );
      }
      plantyring.show();
      delay(25);
    }
  }
  led_godark();
  #endif
}
