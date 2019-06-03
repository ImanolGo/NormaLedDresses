///////////////////////////////////////////////////////////////////
// Class controlling the Serial communication
//
//
// This code is under A Creative Commons Attribution/Share-Alike License
// http://creativecommons.org/licenses/by-sa/4.0/
// 2018, Imanol Gomez
///////////////////////////////////////////////////////////////////


#define HEADER_SIZE 6
#define SIZE_INDEX 3
#define COMMAND_INDEX 4
#define BUFF_SIZE 256

#pragma once
#include "Arduino.h"
#include "RFManager.h"


class SerialManager
{
  private:
 
  RFManager* rfManager;  ///< it sends the data to the lora manager
  

  public:
    
    SerialManager(RFManager* rfManager);
    
    void setup();
    void update();

  private:

    void initializeSerial();
    void recvBytesWithStartEndMarkers();
    void parseNewData();
    void parseMessage(uint8_t* _buffer, uint8_t bufferSize);
    bool isMessage(uint8_t* _buffer, uint8_t bufferSize);
    bool isData(uint8_t* _buffer, uint8_t bufferSize);
    bool isConnection(uint8_t* _buffer, uint8_t bufferSize);
    void sendConnected();
    void sendCommand(uint8_t* _buffer, uint8_t bufferSize);
    void clearSerial();

    bool _connected;
    bool newData;
    uint8_t receivedBytes[BUFF_SIZE];
    byte numReceived = 0;
  
};


SerialManager::SerialManager(RFManager* rfManager)
{
    this->rfManager=rfManager;
    _connected = false;
    newData = false;
}

void SerialManager::setup()
{
    initializeSerial();
    //Serial.println("SerialManager::setup");
    //Serial.print("OK");
}

void SerialManager::initializeSerial()
{
    Serial.begin(115200);
    //Serial.setTimeout(1000);
//    while (!Serial) {
//      delay(1);
//    }
    delay(100);

    this->clearSerial();
    this->sendConnected();

    //Serial.print("SerialManager::started Serial -> ");

   
    
}

void SerialManager::clearSerial()
{ 
   while ( Serial.available()) 
  {
      char byte_ = Serial.read();
  }
}

void SerialManager::update()
{     
  
//    if ( Serial.available()) 
//    {
//        Serial.println("SerialManager::started Serial -> ");
//         delay(50);
//         char buf[BUFF_SIZE];
//         int numBytes = Serial.readBytes(buf,BUFF_SIZE);
//         this->parseMessage(buf, numBytes);
//         this->sendConnected();
//         
//    }

    this->recvBytesWithStartEndMarkers();
    this->parseNewData();
}

void SerialManager::recvBytesWithStartEndMarkers()
{     
      static boolean recvInProgress = false;
      static byte ndx = 0;
      byte startMarker = 0x10;
      byte endMarker = 0xFF;
      byte rb;

       while (Serial.available() > 0 && newData == false) {
       rb = Serial.read();

        if (recvInProgress == true) {
            if (rb != endMarker) {
                receivedBytes[ndx] = rb;
                ndx++;
                if (ndx >= BUFF_SIZE) {
                    ndx = BUFF_SIZE - 1;
                }
            }
            else {
                receivedBytes[ndx] = rb;
                recvInProgress = false;
                numReceived = ndx;  // save the number for use when printing
                ndx = 0;
                newData = true;
                //this->sendConnected();
            }
        }

        else if (rb == startMarker) {
            recvInProgress = true;
            receivedBytes[ndx] = rb;
            ndx++;
        }
    }

    
    
//     if( Serial.available()){
//         uint8_t buf[BUFF_SIZE];
//         uint8_t ending = 255;
//         uint8_t numBytes = Serial.readBytesUntil(ending,buf, BUFF_SIZE);
//         //Serial.write(buf,numBytes);
//         this->parseMessage(buf, numBytes);
//     }
//     uint8_t numBytes = Serial.available();
//    if (numBytes > 0) {
//         //Serial.print("SerialManager::received -> ");
//          uint8_t buf[numBytes];
//         Serial.readBytes((char *) buf,numBytes);
//         //Serial.print("OK");
//         this->parseMessage(buf, numBytes);
//  }
//  
//    uint8_t numBytes = Serial.available();
//    
//    if ( Serial.available()) 
//    {
//         
//         char buf[BUFF_SIZE];
//         int numBytes = Serial.readBytes(buf,BUFF_SIZE);
//         this->parseMessage(buf, numBytes);
//    }
//    
}


void SerialManager::parseNewData() {
    if (newData == true) 
    {
//        Serial.print("This just in (HEX values)... ");
//        for (byte n = 0; n < numReceived; n++) {
//            Serial.print(receivedBytes[n], HEX);
//            Serial.print(' ');
//        }
//        Serial.println();
        
        this->parseMessage(receivedBytes,numReceived);
        newData = false;
    }
}

void SerialManager::parseMessage(uint8_t* buf, uint8_t len)
{
      if(this->isMessage(buf,len))
      {
          if(this->isConnection(buf, len)){
            this->sendConnected();
            
          }
          else if(this->isData(buf, len)){
              this->sendCommand(buf, len);
          } 
         
      }
}


void SerialManager::sendConnected()
{
      int len = HEADER_SIZE+1;
      byte buf[len];
      buf[0] = 0x10;
      buf[1] = 0x41;
      buf[2] = 0x37;
      buf[3] = 1;
      buf[4] = 'c';
      buf[5] = 0;
      buf[6] = 'c';
      
      Serial.write(buf, len);
      //delay(50);
      //this->rfManager->sendMessage(buf, len);
      _connected = true;
}

void SerialManager::sendCommand(uint8_t* buf, uint8_t len)
{
      uint8_t size_ = 2;
      if(len < HEADER_SIZE+size_){
        return;
      }
      
      uint8_t data[size_];
      data[0] = buf[HEADER_SIZE];
      data[1] = buf[HEADER_SIZE+1];
   
//      Serial.print("SerialManager::isMessage -> sendCommand: ");
//      Serial.print(data[0]); Serial.print(" "); Serial.print(data[1]);
      this->rfManager->sendData(data[0], data[1]);
}



bool SerialManager::isMessage(uint8_t* _buffer, uint8_t bufferSize)
{
    if ( _buffer[0] == 0x10 && _buffer[1] == 0x41 && _buffer[2] == 0x37) 
    {   
     
        uint8_t data_size = _buffer[SIZE_INDEX];
        //Serial.print("SerialManager::isMessage -> data size "); Serial.println(data_size);
        if ( (bufferSize-HEADER_SIZE) == data_size ) 
        {
          //Serial.println("SerialManager::isMessage -> true");
   
          return true; 
        }
     
    }

   
    //Serial.println("SerialManager::isMessage -> false");
    return false;
}


bool SerialManager::isData(uint8_t* _buffer, uint8_t bufferSize)
{
    if ( _buffer[COMMAND_INDEX] == 'd') { 
      //Serial.println("SerialManager::isData -> true");
      return true;
    }

     //Serial.println("SerialManager::isData -> false");

    return false;
   
}


bool SerialManager::isConnection(uint8_t* _buffer, uint8_t bufferSize)
{
    if ( _buffer[COMMAND_INDEX] == 'c') { 
       //Serial.println("SerialManager::isConnection -> true");
      return true;
    }

    //Serial.println("SerialManager::isConnection -> false");
    return false;
}
