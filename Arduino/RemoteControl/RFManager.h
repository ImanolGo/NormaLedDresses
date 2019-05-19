///////////////////////////////////////////////////////////////////
// Class controlling the RF Communications
//
//
// This code is under A Creative Commons Attribution/Share-Alike License
// http://creativecommons.org/licenses/by-sa/4.0/
// 2017, Imanol Gomez
///////////////////////////////////////////////////////////////////

#pragma once
#include "Arduino.h"
#include "FastLED.h"
#include <SPI.h>
#include <NRFLite.h>

const static uint8_t RADIO_ID = 1;       // Our radio's id.  The transmitter will send to this id.
const static uint8_t DESTINATION_RADIO_ID = 0; // Id of the radio we will transmit to.
const static uint8_t PIN_RADIO_CE = 10;
const static uint8_t PIN_RADIO_CSN = 9;

struct RadioPacket // Any packet up to 32 bytes can be sent.
{
    uint8_t deviceId;
    uint8_t instruction;
};


class RFManager{

  public:
    
    RFManager();
    
    void setup();
    void update();
    void sendData(int id, int instruction);


  private:

    void setupRF();
    
  private:

    NRFLite _radio;
    RadioPacket _radioData;

};

RFManager::RFManager()
{
   //Intentionally Left Empty
}

void RFManager::setup()
{
    this->setupRF();
}

void RFManager::setupRF()
{
    if (!_radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN))
    {
        Serial.println("RFManager::setupRF -> Cannot communicate with radio");
        while (1); // Wait here forever.
    }

    Serial.println("RFManager::setupRF ->Successfully connected");
    
    /*
    By default, 'init' configures the radio to use a 2MBPS bitrate on channel 100 (channels 0-125 are valid).
    Both the RX and TX radios must have the same bitrate and channel to communicate with each other.
    You can assign a different bitrate and channel as shown below.
    _radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN, NRFLite::BITRATE250KBPS, 0)
    _radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN, NRFLite::BITRATE1MBPS, 75)
    _radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN, NRFLite::BITRATE2MBPS, 100) // THE DEFAULT
    */
}


void RFManager::update()
{
 
}

void RFManager::sendData(int id, int instruction)
{
  _radioData.deviceId = id;
  _radioData.instruction = instruction;

  if (_radio.send(DESTINATION_RADIO_ID, &_radioData, sizeof(_radioData))) // Note how '&' must be placed in front of the variable name.
    {
        Serial.print("RFManager::sendData -> id: ");
        Serial.print(_radioData.deviceId);
        Serial.print(", instruction:  ");
        Serial.println(_radioData.instruction);
    }
    else
    {
       Serial.println("RFManager::sendData -> Could not send data ");
    }
}


   
