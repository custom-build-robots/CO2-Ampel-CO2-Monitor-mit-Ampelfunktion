/* This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details. 
 
 For some more details on that project please visit my blog:
 https://www.byteyourlife.com/top-story/bauanleitung-co2-ampel-fuer-bessere-raumluft-selber-bauen/8430
 
 */
#include <SparkFun_SCD30_Arduino_Library.h>
#include <Wire.h>
#include <ESP8266WiFi.h>

// Library to use the FLASH button to start the calibration of the SCD30 sensor
#include <EasyButton.h>

// Enable/Disable no phone mode (1,0)
#define HAS_NeoPixel_Ring 1
#define HAS_NeoPixel_Stick 1
#define HAS_OLED 0
#define HAS_LED 0


uint32_t sec = 0;
uint32_t sec_old = 0;
uint32_t sec_delta = 0;

#if HAS_NeoPixel_Ring
  //Adafruit NEO Pixel Setup
  #include <Adafruit_NeoPixel.h>
  // define the data pin
  #define PIN D8
  Adafruit_NeoPixel strip = Adafruit_NeoPixel(12, PIN, NEO_GRB + NEO_KHZ800);
  int NUM_LEDS_Ring = strip.numPixels();
#endif

#if HAS_NeoPixel_Stick
  //Adafruit NEO Pixel Setup
  #include <Adafruit_NeoPixel.h>
  // define the data pin
  #define PIN D7
  Adafruit_NeoPixel stick = Adafruit_NeoPixel(8, PIN, NEO_GRB + NEO_KHZ800);
  int NUM_LEDS_Stick = stick.numPixels();
#endif

#if HAS_OLED
  // OLED Display Setup
  #include <SPI.h>
  #include <Wire.h>
  #include "SSD1306Wire.h" // legacy include: `#include "SSD1306.h"`
  // Initialize the OLED display using Wire library
  SSD1306Wire  display(0x3c, D2, D1);  //D2=SDK  D1=SCK  As per labeling on NodeMCU
#endif
  

// Arduino pin where the button is connected to.
#define BUTTON_PIN 0

// Instance of the button.
EasyButton button(BUTTON_PIN);

int green = 1000;
int yellow = 1399;
int red = 1400;
int counter = 1;
int CO2 = 0;
int temperature = 0;
int humidity = 0;
//Reading CO2, humidity and temperature from the SCD30 By: Nathan Seidle SparkFun Electronics 

//https://github.com/sparkfun/SparkFun_SCD30_Arduino_Library

SCD30 airSensorSCD30; // Objekt SDC30 Umweltsensor

void setup(){ // Einmalige Initialisierung
  WiFi.forceSleepBegin(); // Wifi off
  #if HAS_LED  
    pinMode( 12 , OUTPUT);
    pinMode( 13 , OUTPUT);
    pinMode( 14 , OUTPUT);
  #endif

  // ---- Initialisiere den I2C-Bus 
  Wire.begin(); 
  if (Wire.status() != I2C_OK) Serial.println("Something wrong with I2C");
  if (airSensorSCD30.begin() == false) {
    Serial.println("The SCD30 did not respond. Please check wiring."); 
    while(1) {
      yield(); 
      delay(1);
    } 
  }
  
  #if HAS_LED 
    digitalWrite( 12 , LOW );
    digitalWrite( 13 , LOW );
    digitalWrite( 14 , LOW );
  #endif
  
  airSensorSCD30.setAutoSelfCalibration(false); // Sensirion no auto calibration
  airSensorSCD30.setMeasurementInterval(2);     // CO2-Messung alle 5 s

  Serial.begin(115200);
  Serial.println();
  Wire.setClock(100000L);            // 100 kHz SCD30 
  Wire.setClockStretchLimit(200000L);// CO2-SCD30

  // Initialize the button.
  button.begin();
  // Add the callback function to be called when the button is pressed.
  button.onPressed(CO2_Kalibrierfunktion);

  #if HAS_OLED
    // Initialize the OLED Display.
    Serial.println("Initializing OLED Display");
    display.init();
  #endif
  
  #if HAS_NeoPixel_Ring
    // Initialize the NEO Pixel.
    strip.begin();
    strip.setBrightness(60);
    strip.show(); // Initialize all pixels to 'off'
  #endif
  
  
  #if HAS_NeoPixel_Stick
    // Initialize the NEO Pixel.
    stick.begin();
    stick.setBrightness(60);
    stick.show(); // Initialize all pixels to 'off'
  #endif

}

// Calibration
void CO2_Kalibrierfunktion(){ 
  // Forced Calibration Sensirion SCD 30
  #if HAS_OLED
    drawString(String(CO2), String(temperature), String(humidity), "Start Calibration");
  #endif
  #if HAS_NeoPixel_Ring
    setNeoPixel_Ring(240, 3, 252);
    delay( 200 );
    setNeoPixel_Ring(0, 234, 255);
    delay( 200 );
    setNeoPixel_Ring(17, 0, 255);
    delay( 200 );
  #endif
  #if HAS_LED
    for(int i = 0; i < 10; i++){
       digitalWrite( 12 , LOW );
       digitalWrite( 13 , LOW );
       digitalWrite( 14 , LOW );
       delay( 100 );
       digitalWrite( 12 , LOW );
       digitalWrite( 13 , LOW );
       digitalWrite( 14 , HIGH );  
       delay( 100 );
       digitalWrite( 12 , LOW );
       digitalWrite( 13 , HIGH );
       digitalWrite( 14 , HIGH ); 
       delay( 100 );
       digitalWrite( 12 , HIGH );
       digitalWrite( 13 , HIGH );
       digitalWrite( 14 , HIGH );
       delay( 100 );
    }    
  #endif
  Serial.print("Start SCD 30 calibration, please wait 30 s ...");
  delay(30000);
  airSensorSCD30.setAltitudeCompensation(0); // Altitude in m ü NN 
  airSensorSCD30.setForcedRecalibrationFactor(400); // fresh air 
  Serial.println(" done");
  #if HAS_OLED
    drawString(String(CO2), String(temperature), String(humidity), "End Calibration");
  #endif
  #if HAS_NeoPixel_Ring
    setNeoPixel_Ring(240, 3, 252);
    delay( 200 );
    setNeoPixel_Ring(0, 234, 255);
    delay( 200 );
    setNeoPixel_Ring(17, 0, 255);
    delay( 200 );
  #endif  
  #if HAS_LED
    for(int i = 0; i < 10; i++){
       digitalWrite( 12 , LOW );
       digitalWrite( 13 , LOW );
       digitalWrite( 14 , LOW );
       delay( 100 );
       digitalWrite( 12 , HIGH );
       digitalWrite( 13 , LOW );
       digitalWrite( 14 , HIGH );  
       delay( 100 );
       digitalWrite( 12 , LOW );
       digitalWrite( 13 , HIGH );
       digitalWrite( 14 , LOW );
       delay( 100 );
       digitalWrite( 12 , HIGH );
       digitalWrite( 13 , LOW );
       digitalWrite( 14 , HIGH );  
       delay( 100 );
     }  
    digitalWrite( 12 , LOW );
    digitalWrite( 13 , LOW );
    digitalWrite( 14 , LOW );
  #endif    
}

void loop() { // Kontinuierliche Wiederholung 
  CO2 = airSensorSCD30.getCO2() ;
  humidity = airSensorSCD30.getHumidity();
  temperature = airSensorSCD30.getTemperature();

  sec = millis();
  sec = sec / 1000;
  Serial.print("Delta in sec: "); Serial.println((sec-sec_old), 1);

  if(sec_old == 0)
  {
    sec_old = sec;
  }

  sec_delta = sec - sec_old;

  #if HAS_NeoPixel_Stick  
    if ((sec_delta >=0) && (sec_delta<= 150) && (counter == 1))
        {
        counter++;
        setNeoPixel_Stick(255, 0, 0, 1);
    }
  
    if((sec_delta >=150) && (sec_delta<= 300) && (counter == 2))
    {
        counter++;
        setNeoPixel_Stick(255, 0, 0, 2);
    }
    
    if((sec_delta >=300) && (sec_delta<= 450) && (counter == 3))
    {
        counter++;
        setNeoPixel_Stick(255, 0, 0, 3);
    }
  
    if((sec_delta >=450) && (sec_delta<= 600) && (counter == 4))
    {
        counter++;
        setNeoPixel_Stick(255, 0, 0, 4);
    }
  
    if((sec_delta >=600) && (sec_delta<= 750) && (counter == 5))
    {
        counter++;
        setNeoPixel_Stick(255, 0, 0, 5);
    }
  
    if((sec_delta >=750) && (sec_delta<= 900)  && (counter == 6))
    {
        counter++;
        setNeoPixel_Stick(255, 0, 0, 6);
    }
  
    if((sec_delta >=900) && (sec_delta<= 1050) && (counter == 7))
    {
        counter++;
        setNeoPixel_Stick(255, 0, 0, 7);
    }
  
    if((sec_delta >=1050) && (sec_delta<= 1200) && (counter == 8))
    {
        counter++;
        setNeoPixel_Stick(255, 0, 0, 8);
    }
      
    if((sec_delta >=1200) && (sec_delta<= 1500)&& (counter == 9))
    {
        setNeoPixel_Stick(25, 9, 237, 8);
        delay( 500 );
        setNeoPixel_Stick(9, 127, 237, 8);
        delay( 500 );
    }
  
    if(sec_delta > 1500)
    {
      sec_old = sec;
      counter = 1;

    }

  #endif
  
  Serial.print("CO2:"+String(String(CO2)));
  Serial.println();
  if (( ( CO2 ) < ( green ) ))
  {
    #if HAS_LED
      digitalWrite( 12 , LOW );
      digitalWrite( 13 , LOW );
      digitalWrite( 14 , HIGH );
    #endif
  }
  else
  {
    if (( ( CO2 ) < ( yellow ) ))
    {
      #if HAS_LED
        digitalWrite( 12 , LOW );
        digitalWrite( 13 , HIGH );
        digitalWrite( 14 , LOW );
      #endif
    }
    else
    {
      #if HAS_LED
        digitalWrite( 12 , HIGH );
        digitalWrite( 13 , LOW );
        digitalWrite( 14 , LOW );
      #endif
    }
  }

  #if HAS_NeoPixel_Ring
   if((CO2 >= 0) && (CO2 <= 500))
   {
    setNeoPixel_Ring(0, 166, 50);
    Serial.println("#1"); 
   }
   if((CO2 >= 500) && (CO2 <= 700))
   {
    setNeoPixel_Ring(9, 227, 74);
    Serial.println("#2");
   }
   if((CO2 >= 700) && (CO2 <= 900))
   {
    setNeoPixel_Ring(176, 255, 41);
    Serial.println("#3");
   }
   if((CO2 >= 900) && (CO2 <= 1100))
   {
    setNeoPixel_Ring(255, 255, 41);
    Serial.println("#4");
   }
   if((CO2 >= 1100) && (CO2 <= 1300))
   {
    setNeoPixel_Ring(255, 212, 41);
    Serial.println("#5");
   }
   if((CO2 >= 1300) && (CO2 <= 1500))
   {
    setNeoPixel_Ring(255, 152, 41);
    Serial.println("#6");
   }
   if((CO2 >= 1500) && (CO2 <= 1700))
   {
    setNeoPixel_Ring(255, 101, 41);
    Serial.println("#7");
   }
   if((CO2 >= 1700) && (CO2 <= 1900))
   {
    setNeoPixel_Ring(255, 41, 41);
    Serial.println("#8");
   }
  #endif

   

  // Continuously read the status of the button. 
  button.read();
  #if HAS_OLED
    drawString(String(CO2), String(temperature), String(humidity), "");
  #endif
  delay( 1000 );
}


#if HAS_NeoPixel_Ring 
  void setNeoPixel_Ring(int r, int g, int b){
    // Walk through each Neopixel
    for(int i=0; i<NUM_LEDS_Ring; i++) { // For each pixel...
      // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
      strip.setPixelColor(i, strip.Color(r, g, b));
      strip.show();   // Send the updated pixel colors to the hardware.
    } 
  }
#endif

#if HAS_NeoPixel_Stick 
  void setNeoPixel_Stick(int r, int g, int b, int led){
    // Walk through each Neopixel
    for(int i=0; i<NUM_LEDS_Stick; i++) { // For each pixel...
      // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
      stick.setPixelColor(i, stick.Color(0, 0, 255));
      stick.show();   // Send the updated pixel colors to the hardware.
      //Serial.print("LED counter 1: "); Serial.println(i);
    } 
    
    
    for(int i=0; i<led; i++) { // For each pixel...
      // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
      stick.setPixelColor(i, stick.Color(r, g, b));
      stick.show();   // Send the updated pixel colors to the hardware.
      //Serial.print("LED counter 2: "); Serial.println(i);
    } 
  }
#endif

#if HAS_OLED
  // 20201105 Ingmar Stapel
  // Function for drawing a string on the OLED display
  void drawString(String string_1, String string_2, String string_3, String string_4) {
  
    // clear the display
    display.clear();
    delay( 100 );
    // Font Demo1
    // create more fonts at http://oleddisplay.squix.ch/
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(Dialog_bold_16);
    display.drawString(0, 0, "CO2: " + string_1);
    display.setFont(Dialog_bold_16);
    display.drawString(0, 16, "temp:" + string_2);
    display.setFont(Dialog_bold_16);
    display.drawString(0, 32, "hum." + string_3);
    display.setFont(Dialog_bold_16);
    display.drawString(0, 48, string_4);    
    // write the buffer to the display
    display.display();
  }
#endif
