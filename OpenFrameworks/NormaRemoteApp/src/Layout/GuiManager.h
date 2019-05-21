/*
 *  GuiManager.h
 *  NormaRemoteApp
 *
 *  Created by Imanol Gomez on 19/05/19.
 *
 */


#pragma once

#include "Manager.h"
#include "ofxDatGui.h"


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
   // static const int GUI_WIDTH;
    
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
    
    int getWidth() {return m_gui.getWidth();}
    
    int getHeight() {return m_gui.getHeight();}
    
    ofPoint  getPosition() {return m_gui.getPosition();}
    
    void onDropdownEvent(ofxDatGuiDropdownEvent e);
    
    void onColorPickerEvent(ofxDatGuiColorPickerEvent e);
    
    void onButtonEvent(ofxDatGuiButtonEvent e);
    
    void onToggleEvent(ofxDatGuiToggleEvent e);
    
    void onMatrixEvent(ofxDatGuiMatrixEvent e);
    
    void setSerialConnected(bool value);
    
    void setId(int& value);
    
    void setCommand(int& value);
    
    
private:
    
    void setupGuiParameters();
    
    void setupSerialGui();
    
    void drawRectangle();
    
    void drawGui();
    
    void setupGuiEvents();
    
    
private:
    
    ofxDatGui            m_gui;
    
    ofParameter<float>    m_guiFPS;
    
    
    ofParameterGroup      m_parameters;
    
    ofParameter<int>        m_id;
    ofParameter<int>        m_command;
    ofParameter<bool>           m_serialConnected;
   

    
    bool        m_showGui;  //It defines the whether the gui should be shown or not
};

//==========================================================================


