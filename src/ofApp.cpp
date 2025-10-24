#include "ofApp.h"
#include "ofxTimeMeasurements.h"
#include "Mod.hpp"

using namespace ofxMarkSynth;



void ofApp::setup(){
  ofSetBackgroundColor(0);
  ofDisableArbTex();
  ofSetFrameRate(FRAME_RATE);
  glEnable(GL_PROGRAM_POINT_SIZE);

  TIME_SAMPLE_SET_FRAMERATE(FRAME_RATE);
  TIME_SAMPLE_SET_DRAW_LOCATION(TIME_MEASUREMENTS_BOTTOM_RIGHT);
  TIME_SAMPLE_DISABLE(); // ************************************************************************

  glm::vec2 size = { 7200, 7200 };
  synthPtr = createSynthSoftCircle(size);
//  synthPtr = createSynth1(size);
//  synthPtr = createSynth2(size);
  
//  ofLogNotice() << "ofApp::setup synth configured"; // error happens between this and update()
}

//--------------------------------------------------------------
void ofApp::update(){
//  ofLogNotice() << "ofApp::update frame " << ofGetFrameNum();  // error happens before this and after setup()
  synthPtr->update();
}

//--------------------------------------------------------------
void ofApp::draw(){
  synthPtr->draw();
}

void ofApp::drawGui(ofEventArgs& args){
  static glm::vec2 guiWindowSize = {0.0, 0.0};
  auto currentWindowSize = ofGetCurrentWindow()->getWindowSize();
  if (guiWindowSize != currentWindowSize) {
    guiWindowSize = currentWindowSize;
    synthPtr->setGuiSize(guiWindowSize);
  }
  synthPtr->drawGui();
}

//--------------------------------------------------------------
void ofApp::exit(){
  synthPtr->shutdown();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
  if (synthPtr->keyPressed(key)) return;
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
  
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
  
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
  
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
  
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
  
}

//--------------------------------------------------------------
void ofApp::mouseScrolled(int x, int y, float scrollX, float scrollY){
  
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){
  
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){
  
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
  
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
  
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
  
}
