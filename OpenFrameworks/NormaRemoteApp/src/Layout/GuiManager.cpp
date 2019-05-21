/*
 *  GuiManager.cpp
 *  NormaRemoteApp
 *
 *  Created by Imanol Gomez on 19/05/19.
 *
 */


#include "ofMain.h"

#include "AppManager.h"

#include "GuiManager.h"
#include "GuiTheme.h"


const string GuiManager::GUI_SETTINGS_FILE_NAME = "xmls/GuiSettings.xml";
const string GuiManager::GUI_SETTINGS_NAME = "GUI";
//const int GuiManager::GUI_WIDTH = 350;


GuiManager::GuiManager(): Manager(), m_showGui(true)
{
    //Intentionally left empty
}


GuiManager::~GuiManager()
{
    this->saveGuiValues();
    ofLogNotice() <<"GuiManager::Destructor";
}


void GuiManager::setup()
{
    if(m_initialized)
        return;
    
    Manager::setup();
    
    
    this->setupGuiParameters();
    this->setupSerialGui();

    this->setupGuiEvents();
    this->loadGuiValues();

    
    ofLogNotice() <<"GuiManager::initialized";
    
}

void GuiManager::setupGuiParameters()
{
    
    ofxDatGuiLog::quiet();
    
    //m_gui.setPosition(ofxDatGuiAnchor::TOP_LEFT);
    m_gui.setPosition(0,0);
    //m_gui.setAssetPath(ofToDataPath("fonts/"));
    //m_gui.setAssetPath(ofToDataPath("fonts/"));
    //m_gui.setAssetPath("../Resources/data/fonts/");
    m_gui.setTheme(new GuiTheme());
    
    
    int margin =  LayoutManager::MARGIN;
    m_gui.setAutoDraw(false);
    auto pos = m_gui.getPosition();
    m_gui.setPosition(pos.x + margin, pos.y + margin);
    m_gui.addHeader(GUI_SETTINGS_NAME, false);
    
    m_gui.addFRM(0.1);
    
    m_gui.addBreak();
}

void GuiManager::setupSerialGui()
{
    m_id.set("ID", 0, 0, 2);
    m_id.addListener(this, &GuiManager::setId);
    
    m_command.set("Command", 0, 0, 3);
    m_command.addListener(this, &GuiManager::setCommand);
    
    m_serialConnected.set("Serial", AppManager::getInstance().getSerialManager().getConnected());
    
    ofxDatGuiFolder* folder = m_gui.addFolder("Serial", ofColor::purple);
    folder->addToggle("Serial", AppManager::getInstance().getSerialManager().getConnected());
    folder->addSlider(m_id);
    folder->addSlider(m_command);
    folder->expand();
    
    m_gui.addBreak();
}



void GuiManager::setId(int& value)
{
    AppManager::getInstance().getSerialManager().sendRemote(value, m_command);
}

void GuiManager::setCommand(int& value)
{
    AppManager::getInstance().getSerialManager().sendRemote(m_id, value);
}

void GuiManager::update()
{
    m_gui.update();
}


void GuiManager::draw()
{
    if(!m_showGui)
        return;
    
    //this->drawRectangle();
    this->drawGui();
}


void GuiManager::drawGui()
{
    ofEnableAlphaBlending();
    m_gui.draw();
    ofDisableAlphaBlending();
}

void GuiManager::setupGuiEvents()
{
    //m_gui.onDropdownEvent(this, &GuiManager::onDropdownEvent);
    //m_gui.onColorPickerEvent(this, &GuiManager::onColorPickerEvent);
    m_gui.onButtonEvent(this, &GuiManager::onButtonEvent);
    //m_gui.onToggleEvent(this, &GuiManager::onToggleEvent);
    //m_gui.onMatrixEvent(this, &GuiManager::onMatrixEvent);
}


void GuiManager::saveGuiValues(string path)
{
    ofXml xml;
    ofSerialize(xml, m_parameters);
    //xml.serialize(m_parameters);
    
    if(path.empty()){
        xml.save(GUI_SETTINGS_FILE_NAME);
    }
    else{
        xml.save(path);
    }
    
    
}

void GuiManager::loadGuiValues(string path)
{
    ofXml xml;
    if(path.empty()){
         xml.load(GUI_SETTINGS_FILE_NAME);
    }
    else{
         xml.load(path);
    }
    
    //xml.deserialize(m_parameters);
    ofDeserialize(xml, m_parameters);
}



void GuiManager::toggleGui()
{
    m_showGui = !m_showGui;
}

void GuiManager::drawRectangle()
{
    int margin =  LayoutManager::MARGIN;
    ofPushStyle();
    ofSetColor(15);
    ofDrawRectangle( m_gui.getPosition().x - margin, 0, m_gui.getWidth() + 2*margin, ofGetHeight());
    ofPopStyle();
}



void GuiManager::onDropdownEvent(ofxDatGuiDropdownEvent e)
{
    cout << "onDropdownEvent: " << e.target->getName() << " Selected" << endl;
    
}

void GuiManager::onColorPickerEvent(ofxDatGuiColorPickerEvent e)
{
    cout << "onColorPickerEvent: " << e.target->getName() << " Selected" << endl;

   
}

void GuiManager::onButtonEvent(ofxDatGuiButtonEvent e)
{
    cout << "onButtonEvent: " << e.target->getName() << " Selected" << endl;
    
}


void GuiManager::onToggleEvent(ofxDatGuiToggleEvent e)
{
    cout << "onToggleEvent: " << e.target->getName() << " Selected" << endl;
    
}

void GuiManager::onMatrixEvent(ofxDatGuiMatrixEvent e)
{
    cout << "onMatrixEvent " << e.child << " : " << e.enabled << endl;
}


void GuiManager::setSerialConnected(bool value)
{
    m_gui.getToggle("Serial")->setChecked(value);
}

