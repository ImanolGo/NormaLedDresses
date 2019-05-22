
 /*
  
  RF Serial Sender 
     
 Description:
 * Firmware sending Serial changes using the an RF module
 Software:
 * RFLite
 
 Hardware:
* Arduino Nano
* NRF24L01+ module


 created 19 May 2019
 This code is under A Creative Commons Attribution/Share-Alike License http://creativecommons.org/licenses/by-sa/4.0/
   (2019) by Imanol Gomez

 */
 

#include "RFManager.h"
#include "SerialManager.h"


RFManager rfManager;
SerialManager serialManager(&rfManager);
RadioPacket radioData;

void setup() 
{
    serialManager.setup();
    rfManager.setup();
}

void loop() 
{
    rfManager.update();
    serialManager.update();
 
}
