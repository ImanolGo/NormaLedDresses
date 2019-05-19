///////////////////////////////////////////////////////////////////
// Class managing the gesture interpreted from the buttons Events
//
// Based on ClickButton Class from raron
// This code is under A Creative Commons Attribution/Share-Alike License
// http://creativecommons.org/licenses/by-sa/4.0/
// 2017, Imanol Gomez
///////////////////////////////////////////////////////////////////

#pragma once
#include "Arduino.h"
#include "RFManager.h"
#include "ButtonEvents.h"

#define NUM_BUTTONS 4

class ButtonEventManager{
public:

    ButtonEventManager(RFManager* rfManager);
    
    /// set all information necessary to use the module and initialize it
    void setup();

    void update();

  private:

    void setupButtons();

    void updateButtons();

    void checkButtons();
    
  private:
    
    ButtonEvents _butttons[NUM_BUTTONS];
    RFManager* rfManager;   
};


ButtonEventManager::ButtonEventManager(RFManager* rfManager)
{
  this->rfManager=rfManager;
}



void ButtonEventManager::setup()
{
    this->setupButtons();
}

void ButtonEventManager::setupButtons()
{
     pinMode(2, INPUT_PULLUP); 
    _butttons[0] = ButtonEvents();
    _butttons[0].attach(2);

     pinMode(3, INPUT_PULLUP); 
    _butttons[1] = ButtonEvents();
    _butttons[1].attach(3);

     pinMode(4, INPUT_PULLUP); 
    _butttons[2] = ButtonEvents();
    _butttons[2].attach(4);

     pinMode(5, INPUT_PULLUP); 
    _butttons[3] = ButtonEvents();
    _butttons[3].attach(5);
}

void ButtonEventManager::update()
{
    this->updateButtons();
    this->checkButtons();
}

void ButtonEventManager::updateButtons()
 {
      for(int i=0; i<NUM_BUTTONS; i++){
         _butttons[i].update();
      }
 }

void ButtonEventManager::checkButtons()
 {
      for(int i=0; i<NUM_BUTTONS; i++){
         if (_butttons[i].tapped() == true) {
              Serial.print("RFManager::sendData -> TAP detected: "); 
              Serial.println(i);
              this->rfManager->sendData(0,i);        
          } 
      }
 }


 
