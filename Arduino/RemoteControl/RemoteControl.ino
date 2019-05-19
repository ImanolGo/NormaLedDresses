/*
  Norma Remote Control 
   
 Description:
 * Software used to control remotely an RF receiver

 Software:
 * Bounce2
 * ButtonEvents
 * RFLite
 
 Hardware:
* Arduino Nano
* NRF24L01+ module 
   
 created 16 May 2019
 This code is under A Creative Commons Attribution/Share-Alike License http://creativecommons.org/licenses/by-sa/4.0/
   (2019) by Imanol Gomez

 
 */
 

#include "ButtonEventManager.h"
#include "RFManager.h"


RFManager rfManager;
ButtonEventManager buttonEventManager(&rfManager);


void setup() {
  
    Serial.begin(115200);
    delay(300);

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  
  
    rfManager.setup();
    buttonEventManager.setup();
}

void loop() {

    rfManager.update();
    buttonEventManager.update();
   
}
