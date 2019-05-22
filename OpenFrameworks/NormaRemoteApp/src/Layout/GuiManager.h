/*
 *  GuiManager.h
 *  NormaRemoteApp
 *
 *  Created by Imanol Gomez on 19/05/19.
 *
 */


#pragma once

#include "Manager.h"
#include "ofxImGui.h"


//========================== class GuiManager ==============================
//============================================================================
/** \class GuiManager GuiManager.h
 *    \brief Class managing the application´s grapical user interface (GUI)
 *    \details It creates the gui and the callback functions that will be affected
 *    by the gui
 */

class GuiManager: public Manager
{
    static const string GUI_SETTINGS_FILE_NAME;
    static const string GUI_SETTINGS_NAME;
    static const int GUI_WIDTH;
    
public:
    
    //! Constructor
    GuiManager();
    
    //! Destructor
    ~GuiManager();
    
    //! Set-up the gui
    void setup();
    
    //! update the gui
    void update();
    
    //! Draw the gui
    void draw();
    
    void saveGuiValues(string path = "");
    
    void loadGuiValues(string path = "");
    
    void toggleGui();
    
    void showGui(bool show){m_showGui=show;}
    
    int getWidth() {return m_width;}
    
    int getHeight() {return m_height;}
    
    ofPoint  getPosition() {return m_position;}
    
    void setSerialConnected(bool value) {m_serialConnected = value;}
    
    void sendNorma(int& value);
    
    void sendAdalgisa(int& value);
    
    void setId(int& value);
    
    void setCommand(int& value);
    
    
private:
    
    void setupGuiParameters();
    
    void setupNorma();
    
    void setupAdalgisa();
    
    void setupSerialGui();
    
    void drawRectangle();
    
    void drawGui();
    
    void updateSize(const ofxImGui::Settings& settings);
    
    
private:
    
    ofxImGui::Gui   m_gui;
    
    ofParameterGroup      m_parameters;
    
    ofParameterGroup    m_normaGroup;
    ofParameterGroup    m_adalgisaGroup;
    
    ofParameter<int>        m_normaMode;
    ofParameter<int>        m_adalgisaMode;
    ofParameter<int>        m_id;
    ofParameter<int>        m_command;
    ofParameter<bool>       m_serialConnected;
    
    std::vector<std::string>    m_normaNames;
    std::vector<std::string>    m_adalgisaNames;
   
    float m_width, m_height;
    ofPoint   m_position;
    
    bool        m_showGui;  //It defines the whether the gui should be shown or not
};

//==========================================================================

