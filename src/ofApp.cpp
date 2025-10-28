#include "ofApp.h"
#include "ofxTimeMeasurements.h"
#include "Mod.hpp"

using namespace ofxMarkSynth;



void ofApp::setup(){
//  ofSetLogLevel(OF_LOG_VERBOSE);
  
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
  
  // >>> TODO: refactor the MIDI controller setup into a separate class when we know more about it
  lc.listDevices();
  //lc.setup(1);
  if (lc.setup()) { // setup with automatic id finding
    
    // Global agency knob
    lc.knob(0, synthPtr->findParameterByNamePrefix("Global Agency")->get().cast<float>());

    // Bind faders to layer alpha parameters
    auto layersParameterGroupOpt = synthPtr->findParameterByNamePrefix("Layers");
    ofParameterGroup& layerParameters = layersParameterGroupOpt->get().castGroup();
    for (size_t i = 0; i < layerParameters.size(); ++i) {
      ofParameter<float>& layerParameter = layerParameters.getFloat(i);
      lc.fader(i, layerParameter);
    };

    // Bind knobs to audio analysis parameters
    lc.knob(4, synthPtr->findParameterByNamePrefix("MinPitch")->get().cast<float>());
    lc.knob(5, synthPtr->findParameterByNamePrefix("MaxPitch")->get().cast<float>());
    lc.knob(12, synthPtr->findParameterByNamePrefix("MinRms")->get().cast<float>());
    lc.knob(13, synthPtr->findParameterByNamePrefix("MaxRms")->get().cast<float>());
    lc.knob(6, synthPtr->findParameterByNamePrefix("MinComplexSpectralDifference")->get().cast<float>());
    lc.knob(7, synthPtr->findParameterByNamePrefix("MaxComplexSpectralDifference")->get().cast<float>());
    lc.knob(14, synthPtr->findParameterByNamePrefix("MinSpectralCrest")->get().cast<float>());
    lc.knob(15, synthPtr->findParameterByNamePrefix("MaxSpectralCrest")->get().cast<float>());
    lc.knob(22, synthPtr->findParameterByNamePrefix("MinZeroCrossingRate")->get().cast<float>());
    lc.knob(23, synthPtr->findParameterByNamePrefix("MaxZeroCrossingRate")->get().cast<float>());
  }
  // <<<

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
  lc.close(); // close midi ports
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
