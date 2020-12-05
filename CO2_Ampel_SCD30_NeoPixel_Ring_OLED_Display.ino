/* This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details. */
#include <SparkFun_SCD30_Arduino_Library.h>
#include <Wire.h>
#include <ESP8266WiFi.h>

// Library to use the FLASH button to start the calibration of the SCD30 sensor
#include <EasyButton.h>

//Adafruit NEO Pixel Setup
#include <Adafruit_NeoPixel.h>
// define the data pin
#define PIN D8
Adafruit_NeoPixel strip = Adafruit_NeoPixel(12, PIN, NEO_GRB + NEO_KHZ800);
int NUM_LEDS = strip.numPixels();

int lastindex = 0;
byte randLEDNumber = 0;

// OLED Display Setup
#include <SPI.h>
#include <Wire.h>
#include "SSD1306Wire.h" // legacy include: `#include "SSD1306.h"`
// Initialize the OLED display using Wire library
SSD1306Wire  display(0x3c, D2, D1);  //D2=SDK  D1=SCK  As per labeling on NodeMCU

  

// Arduino pin where the button is connected to.
#define BUTTON_PIN 0

// Instance of the button.
EasyButton button(BUTTON_PIN);

int green = 1000;
int yellow = 1399;
int red = 1400;

int CO2 = 0 ;
int temperature = 0 ;
int humidity = 0 ;
//Reading CO2, humidity and temperature from the SCD30 By: Nathan Seidle SparkFun Electronics 

//https://github.com/sparkfun/SparkFun_SCD30_Arduino_Library

SCD30 airSensorSCD30; // Objekt SDC30 Umweltsensor

void setup(){ // Einmalige Initialisierung
  WiFi.forceSleepBegin(); // Wifi off
  pinMode( 12 , OUTPUT);
  pinMode( 13 , OUTPUT);
  pinMode( 14 , OUTPUT);
  Wire.begin(); // ---- Initialisiere den I2C-Bus 

  if (Wire.status() != I2C_OK) Serial.println("Something wrong with I2C");

  if (airSensorSCD30.begin() == false) {
    Serial.println("The SCD30 did not respond. Please check wiring."); 
    while(1) {
      yield(); 
      delay(1);
    } 
  }

  airSensorSCD30.setAutoSelfCalibration(false); // Sensirion no auto calibration

  airSensorSCD30.setMeasurementInterval(2);     // CO2-Messung alle 5 s

  Serial.begin(115200);
  Serial.println();
  digitalWrite( 12 , LOW );

  digitalWrite( 13 , LOW );

  digitalWrite( 14 , LOW );

  Wire.setClock(100000L);            // 100 kHz SCD30 
  Wire.setClockStretchLimit(200000L);// CO2-SCD30

  // Initialize the button.
  button.begin();
  // Add the callback function to be called when the button is pressed.
  button.onPressed(CO2_Kalibrierfunktion);

  // Initialize the OLED Display.
  Serial.println("Initializing OLED Display");
  display.init();



  // Initialize the NEO Pixel.
  strip.begin();
  strip.setBrightness(60);
  strip.show(); // Initialize all pixels to 'off'
   
}






// Kalibrierfunktion
void CO2_Kalibrierfunktion(){ 
  // Forced Calibration Sensirion SCD 30

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
  Serial.print("Start SCD 30 calibration, please wait 30 s ...");
  delay(30000);
  airSensorSCD30.setAltitudeCompensation(0); // Altitude in m ü NN 
  airSensorSCD30.setForcedRecalibrationFactor(400); // fresh air 
  Serial.println(" done");
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
}

void loop() { // Kontinuierliche Wiederholung 
  CO2 = airSensorSCD30.getCO2() ;
  humidity = airSensorSCD30.getHumidity();
  temperature = airSensorSCD30.getTemperature();
  
  Serial.print("CO2:"+String(String(CO2)));
  Serial.println();
  if (( ( CO2 ) < ( green ) ))
  {
    digitalWrite( 12 , LOW );
    digitalWrite( 13 , LOW );
    digitalWrite( 14 , HIGH );
    Serial.print("green");
    Serial.println();

    // Walk through each Neopixel
    for(int i=0; i<NUM_LEDS; i++) { // For each pixel...
      // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
      strip.setPixelColor(i, strip.Color(3, 252, 61));
      strip.show();   // Send the updated pixel colors to the hardware.
    }
    
  }
  else
  {
    if (( ( CO2 ) < ( yellow ) ))
    {
      digitalWrite( 12 , LOW );
      digitalWrite( 13 , HIGH );
      digitalWrite( 14 , LOW );
      Serial.print("yellow");
      Serial.println();
 
      // Walk through each Neopixel
      for(int i=0; i<NUM_LEDS; i++) { // For each pixel...
        // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
        strip.setPixelColor(i, strip.Color(236, 252, 3));
        strip.show();   // Send the updated pixel colors to the hardware.
      }
  
      
    }
    else
    {
      digitalWrite( 12 , HIGH );
      digitalWrite( 13 , LOW );
      digitalWrite( 14 , LOW );
      Serial.print("red");
      Serial.println();

      // Walk through each Neopixel
      for(int i=0; i<NUM_LEDS; i++) { // For each pixel...
        // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
        strip.setPixelColor(i, strip.Color(252, 0, 0));
        strip.show();   // Send the updated pixel colors to the hardware.
      }      
    }
  }

  // Continuously read the status of the button. 
  button.read();
  drawString(String(CO2), String(temperature), String(humidity), "");
  delay( 2000 );
}

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
//  display.setFont(Dialog_bold_16);
//  display.drawString(0, 48, string_4);    
  // write the buffer to the display
  display.display();
}
