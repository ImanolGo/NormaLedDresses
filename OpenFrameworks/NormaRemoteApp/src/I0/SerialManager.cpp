/*
 *  SerialManager.cpp
 *  NormaRemoteApp
 *
 *  Created by Imanol Gomez on 19/05/19.
 *
 */




#include "SerialManager.h"
#include "AppManager.h"


const int SerialManager::BAUD_RATE = 115200;

SerialManager::SerialManager(): Manager(), m_connected(false)
{
    //Intentionally left empty
}

SerialManager::~SerialManager()
{
    ofLogNotice() << "SerialManager::destructor";
}


//--------------------------------------------------------------

void SerialManager::setup()
{
    if(m_initialized)
        return;
    
    Manager::setup();
    
    this->setupHeaders();
    this->setupSerial();
    
    ofLogNotice() <<"SerialManager::initialized" ;
}


void SerialManager::setupHeaders()
{
    m_dataHeader.f1 = 0x10;
    m_dataHeader.f2 = 0x41;
    m_dataHeader.f3 = 0x37;
    m_dataHeader.size = 0;
    m_dataHeader.command = 'd';
    m_dataHeader.channel = 1;
    m_dataHeader.end = 0xFF;
    
    m_connectHeader.f1 = 0x10;
    m_connectHeader.f2 = 0x41;
    m_connectHeader.f3 = 0x37;
    m_connectHeader.size = 1;
    m_connectHeader.command = 'c';
    m_connectHeader.channel = 1;
    m_connectHeader.end = 0xFF;
}


void SerialManager::setupSerial()
{
   // ofLogNotice() <<"SerialManager::setupSerial << Autoconnecting serial port";
    //this->autoConnect();
    
    int serialPort = AppManager::getInstance().getSettingsManager().getSerialPort();
    if(serialPort<0){
        ofLogNotice() <<"SerialManager::setupSerial << Autoconnecting serial port";
        this->autoConnect();
    }
    else{
        ofLogNotice() <<"SerialManager::setupSerial << Connecting serial to port " << serialPort;
        this->connect(serialPort);
    }
}

void SerialManager::connect(int portNum)
{
    std::vector<ofx::IO::SerialDeviceInfo> devicesInfo = ofx::IO::SerialDeviceUtils::listDevices();
    if(devicesInfo.size() < portNum){
        return;
        
    }
    
    if(m_serial.setup(devicesInfo[portNum], BAUD_RATE)) //open a device number
    {
        ofLogNotice() <<"SerialManager::connect << Arduino connected to " << portNum;
        m_connected = true;
    }
    else{
         m_connected = false;
    }
    
    AppManager::getInstance().getGuiManager().setSerialConnected(m_connected);
}

void SerialManager::autoConnect()
{
//    m_serial.listDevices();
//    vector <ofSerialDeviceInfo> deviceList = m_serial.getDeviceList();
//
//    // this should be set to whatever com port your serial device is connected to.
//    // (ie, COM4 on a pc, /dev/tty.... on linux, /dev/tty... on a mac)
//    // arduino users check in arduino app....
//
//    m_connected = false;
//
//    for(auto device: deviceList)
//    {
//        if(this->checkConnection(device.getDeviceID())) //open a device number
//        {
//            ofLogNotice() <<"SerialManager::setupSerial << Arduino connected to port " << device.getDeviceName();
//            m_connected = true;
//            return;
//        }
//    }
    
    std::vector<ofx::IO::SerialDeviceInfo> devicesInfo = ofx::IO::SerialDeviceUtils::listDevices();
    
    for (std::size_t i = 0; i < devicesInfo.size(); ++i)
    {
        ofLogNotice("ofApp::autoConnect -> ") << "\t" << devicesInfo[i];
    }
    
    m_connected = false;
    
    for (auto device: devicesInfo)
    {
        bool valid_port = !(ofIsStringInString( device.port(), "MINIJAMBOX") || ofIsStringInString(device.port(), "Bluetooth") || ofIsStringInString(device.port(),"UEROLL2"));
        if(valid_port&&this->checkConnection(device.port())) //open a device number
        {
            ofLogNotice() <<"SerialManager::setupSerial << Arduino connected to port " << device.description();
            m_connected = true;
            return;
        }
    }
    
    AppManager::getInstance().getGuiManager().setSerialConnected(m_connected);
}


bool SerialManager::checkConnection(string port)
{
    if(m_serial.setup(port, BAUD_RATE)) //open a device number
    {
//        m_serial.flush();
//        ofSleepMillis(500);
        ofLogNotice() <<"SerialManager::checkConnection << Arduino sendConnection to " << port;
        
        this->sendConnection();
        ofSleepMillis(300);
        if(this->receivedConnected()){
            ofLogNotice() <<"SerialManager::checkConnection << Arduino connected to " << port;
            return true;
        }
        else {
            m_serial.close();
        }
    }
    
    ofLogNotice() <<"SerialManager::checkConnection << Arduino is not connected ";
    //m_serial.setup(0, BAUD_RATE);
    return false;
    
}

void SerialManager::sendConnection()
{
    unsigned char connected = 'c';
    
    string message="";
    message+= m_connectHeader.f1; message+= m_connectHeader.f2; message+= m_connectHeader.f3;
    message+=  m_connectHeader.size;
    message+=m_connectHeader.command;
    message+=m_connectHeader.channel;
    message+=connected;
    message+=m_connectHeader.end;

    std::stringstream ss;
    for(int i=0; i<message.length(); ++i){
         ss << std::hex << (int)message[i] << " ";
    }
    std::string mystr = ss.str();
    
    
    ofLogNotice() <<"SerialManager::sendConnection << Sent: " << mystr;
    
    m_serial.writeBytes((unsigned char *) message.c_str(), message.length());
}

bool SerialManager::receivedConnected()
{
    ofLogNotice() <<"SerialManager::checkConnection << Serial Available: " << m_serial.available();
    
    
    int numBytes = m_serial.available();
    
    if ( numBytes == 0 )
    {
        return false;
    }
    
    
    unsigned char bytes[numBytes];
    
    int result = m_serial.readBytes( bytes, numBytes );
    
//    string str((char *)bytes);
    
    std::stringstream ss;
    for(int i=0; i<numBytes; ++i)
        ss << std::hex << (int)bytes[i] << " ";
    std::string mystr = ss.str();
    
    string str((char *)bytes);
    
    ofLogNotice() <<"SerialManager::checkConnection << Received: " << mystr;
    ofLogNotice() <<"SerialManager::checkConnection << Received: " << str;
    
    // check for error code
    if ( result == OF_SERIAL_ERROR ){
        // something bad happened
        ofLog( OF_LOG_ERROR, "unrecoverable error reading from serial" );
    }
    else if ( result == OF_SERIAL_NO_DATA ){
        ofLog( OF_LOG_ERROR, "no data reading from serial" );
    }
    
    
    return (this->isMessage(bytes, numBytes) && this->isConnected(bytes, numBytes));
}


bool SerialManager::isMessage(unsigned char * buffer, int size)
{
    
    if(buffer[0] != m_connectHeader.f1 ){
         ofLogNotice() <<"SerialManager::buffer[0] not 0x10 ";
    }

    if(buffer[1] != m_connectHeader.f2 ){
    ofLogNotice() <<"SerialManager::buffer[1] not 0x41 ";
    }

    if(buffer[2] != m_connectHeader.f3 ){
    ofLogNotice() <<"SerialManager::buffer[2] not 0x37 ";
    }
    
    if(buffer[0] != m_connectHeader.f1  && buffer[1] != m_connectHeader.f2  && buffer[2] != m_connectHeader.f3 ){
        ofLogNotice() <<"SerialManager::isMessage -> FALSE ";
        return false;
    }
    
    
    
    
    ofLogNotice() <<"SerialManager::isMessage -> TRUE ";
    return true;
}

bool SerialManager::isConnected(unsigned char * buffer, int size)
{
    if(buffer[4] == 'c')
    {
        ofLogNotice() <<"SerialManager::isConnected -> TRUE ";
        return true;
    }
    
    
    ofLogNotice() <<"SerialManager::isConnected -> FALSE ";
    return false;
}



void SerialManager::update()
{
     int numBytes = m_serial.available();
     while(numBytes>0){
        
         unsigned char bytes[numBytes];
         m_serial.readBytes( bytes, numBytes );
         
         string str((char *)bytes);
         std::stringstream ss;
         
         for(int i=0; i<str.length(); ++i){
             ss << std::hex << (int)str[i] << " ";
         }
         std::string mystr = ss.str();
         
         ofLogNotice() <<"SerialManager::update ->  " << mystr;
        
         
        numBytes = m_serial.available();
    }
}



void SerialManager::sendRemote(char  id, char command)
{
    if(!m_connected){
        return;
    }

    
    ofLogNotice() <<"SerialManager::sendRemote ->  id: " << int(id) << ", command: " <<  int(command);

    string message="";
    message+= m_dataHeader.f1; message+= m_dataHeader.f2; message+= m_dataHeader.f3;
    m_dataHeader.size = 2;
    message+=  m_dataHeader.size;
    message+=m_dataHeader.command;
    message+=m_dataHeader.channel;
    
    message+=id;
    message+=command;
    message+=m_dataHeader.end;
    
    m_serial.writeBytes((unsigned char *) message.c_str(), message.length());
    
    std::stringstream ss;
    for(int i=0; i<message.length(); ++i){
        ss << std::hex << (int)message[i] << " ";
    }
    std::string mystr = ss.str();
    
    ofLogNotice() <<"SerialManager::sendRemote ->  hex: " << mystr;
    
}



