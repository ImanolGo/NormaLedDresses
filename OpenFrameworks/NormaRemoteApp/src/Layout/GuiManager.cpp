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
const int GuiManager::GUI_WIDTH = 400;


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
    this->setupNorma();
    this->setupAdalgisa();
    this->loadGuiValues();

    
    ofLogNotice() <<"GuiManager::initialized";
    
}

void GuiManager::setupGuiParameters()
{
    
    //create an actual ImGui context before setting up the addon
    ImGui::CreateContext();
    
    //specify a font to use
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontFromFileTTF(&ofToDataPath("fonts/roboto/Roboto-Regular.ttf")[0], 20.f);
    
    m_gui.setup(new GuiTheme());
    ofxImGui::Settings().windowPos  = ofVec2f(0,0);
    ofxImGui::Settings().windowSize = ofVec2f(GUI_WIDTH,ofGetHeight());
    
    m_width = 0;
    m_height = 0;
    
}

void GuiManager::setupSerialGui()
{
    m_serialConnected.set("Serial Connected", false);
}

void GuiManager::setupNorma()
{    
    m_normaGroup.setName("NORMA");
    m_normaMode.set("Scene", 0);
    m_normaMode.addListener(this, &GuiManager::sendNorma);
    m_normaGroup.add(m_normaMode);
    
    for(int i=0; i< 6; i++){
        m_normaNames.push_back(ofToString(i));
    }
}

void GuiManager::setupAdalgisa()
{
    m_adalgisaGroup.setName("ADALGISA");
    m_adalgisaMode.set("Scene", 0);
    m_adalgisaMode.addListener(this, &GuiManager::sendAdalgisa);
    m_adalgisaGroup.add(m_adalgisaMode);
    
    for(int i=0; i< 6; i++){
        m_adalgisaNames.push_back(ofToString(i));
    }
}



void GuiManager::sendNorma(int& value)
{
    AppManager::getInstance().getSerialManager().sendRemote(0, value);
}

void GuiManager::sendAdalgisa(int& value)
{
    AppManager::getInstance().getSerialManager().sendRemote(1, value);
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
    m_gui.setTheme(new GuiTheme());
    m_serialConnected = AppManager::getInstance().getSerialManager().getConnected();
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
    
    m_gui.begin();
    
    auto mainSettings  = ofxImGui::Settings();
    //ofxImGui::Settings().windowPos  = ofVec2f(-LayoutManager::MARGIN,-LayoutManager::MARGIN);
    // ofxImGui::Settings().windowSize = ofVec2f(GUI_WIDTH,ofGetHeight());
    if (ofxImGui::BeginWindow("Norma Remote App", mainSettings, false))
    {
        ImGui::Text("%.1f FPS (%.3f ms/frame)", ofGetFrameRate(), 1000.0f / ImGui::GetIO().Framerate);
        
             ofxImGui::AddParameter(m_serialConnected);
        
        if (ofxImGui::BeginTree(m_normaGroup, mainSettings))
        {
            //ofxImGui::AddCombo(m_normaMode, m_normaNames);
            ofxImGui::AddRadio(m_normaMode, m_normaNames, m_normaNames.size());
            ofxImGui::EndTree(mainSettings);
        }
        
        if (ofxImGui::BeginTree(m_adalgisaGroup, mainSettings))
        {
            //ofxImGui::AddCombo(m_adalgisaMode, m_adalgisaNames);
            ofxImGui::AddRadio(m_adalgisaMode, m_adalgisaNames, m_adalgisaNames.size());
            ofxImGui::EndTree(mainSettings);
        }
        
//        if (ofxImGui::BeginTree(m_communicationsGroup, mainSettings))
//        {
//            ofxImGui::AddParameter(m_streaming);
//            if (ImGui::Button("Next Frame..."))
//            {
//                AppManager::getInstance().getUdpManager().nextFrame();
//            }
//
//            ofxImGui::EndTree(mainSettings);
//
//
//        }
    
    }
    
    ofxImGui::EndWindow(mainSettings);
    m_gui.end();
    
    //ofDisableAlphaBlending();
    
    this->updateSize(mainSettings);
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
    ofPushStyle();
    ofSetColor(15);
    ofDrawRectangle( 0, 0, this->getWidth(), ofGetHeight());
    ofPopStyle();
}



void GuiManager::updateSize(const ofxImGui::Settings& settings)
{
    if(m_width!= settings.windowSize.x){
        m_width = settings.windowSize.x;
        AppManager::getInstance().getLayoutManager().windowResized(ofGetWidth(), ofGetHeight());
    }
    
    m_height = settings.windowSize.y;
    m_position = ofPoint(settings.windowPos.x, settings.windowPos.y);
}
