/*
 *  NormaRemoteApp.cpp
 *  NormaRemoteApp
 *
 *  Created by Imanol Gomez on 19/05/19.
 *
 */



#include "AppManager.h"

#include "NormaRemoteApp.h"

//--------------------------------------------------------------
void NormaRemoteApp::setup(){
    AppManager::getInstance().setup();
}

//--------------------------------------------------------------
void NormaRemoteApp::update(){
    AppManager::getInstance().update();
}

//--------------------------------------------------------------
void NormaRemoteApp::draw(){
    AppManager::getInstance().draw();
}

void NormaRemoteApp::exit()
{
    ofLogNotice() <<"NormaRemoteApp::exit";

}

//--------------------------------------------------------------
void NormaRemoteApp::keyPressed(int key){

}

//--------------------------------------------------------------
void NormaRemoteApp::keyReleased(int key){

}

//--------------------------------------------------------------
void NormaRemoteApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void NormaRemoteApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void NormaRemoteApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void NormaRemoteApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void NormaRemoteApp::windowResized(int w, int h){
    AppManager::getInstance().getLayoutManager().windowResized(w,h);


}

//--------------------------------------------------------------
void NormaRemoteApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void NormaRemoteApp::dragEvent(ofDragInfo dragInfo){

}
