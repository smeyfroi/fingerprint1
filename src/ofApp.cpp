#include "ofApp.h"

using namespace ofxMarkSynth;

void ofApp::configSynth1() {
  synth.configure(createFboConfigs1(), createMods1(), ofGetWindowSize());
  synth.name = "Synth1";
}

void ofApp::configSynth2() {
  synth.configure(createFboConfigs2(), createMods2(), ofGetWindowSize());
  synth.name = "Synth2";
}

void ofApp::setup(){
  ofSetBackgroundColor(0);
  ofDisableArbTex();
  ofSetFrameRate(30);
  
//      audioAnalysisClientPtr = std::make_shared<ofxAudioAnalysisClient::LocalGistClient>(rootSourceMaterialPath/"20250208-violin-separate-scale-vibrato-harmonics.wav");
//      audioAnalysisClientPtr = std::make_shared<ofxAudioAnalysisClient::LocalGistClient>(rootSourceMaterialPath/"Alex Petcu Bell Plates.wav");
  //    audioAnalysisClientPtr = std::make_shared<ofxAudioAnalysisClient::LocalGistClient>(rootSourceMaterialPath/"Alex Petcu Sound Bath.wav");
//  audioAnalysisClientPtr = std::make_shared<ofxAudioAnalysisClient::LocalGistClient>(rootSourceMaterialPath/"20250208-trombone-melody.wav");
//  audioAnalysisClientPtr = std::make_shared<ofxAudioAnalysisClient::LocalGistClient>(rootSourceMaterialPath/"nightsong.wav");
//  audioAnalysisClientPtr = std::make_shared<ofxAudioAnalysisClient::LocalGistClient>(rootSourceMaterialPath/"treganna.wav");

  auto recordingPath = saveFilePath("audio-recordings");
  std::filesystem::create_directory(recordingPath);
  audioAnalysisClientPtr = std::make_shared<ofxAudioAnalysisClient::LocalGistClient>("Apple Inc.: MacBook Pro Microphone", RECORD_AUDIO, recordingPath);

  audioDataProcessorPtr = std::make_shared<ofxAudioData::Processor>(audioAnalysisClientPtr);
  audioDataProcessorPtr->setDefaultValiditySpecs();
  audioDataPlotsPtr = std::make_shared<ofxAudioData::Plots>(audioDataProcessorPtr);
  
//  configSynth1();
  configSynth2();
}

//--------------------------------------------------------------
void ofApp::update(){
  audioDataProcessorPtr->update();
  synth.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
  synth.draw();
  audioDataPlotsPtr->drawPlots();
}

void ofApp::drawGui(ofEventArgs& args){
  static glm::vec2 guiWindowSize = {0.0, 0.0};
  auto currentWindowSize = ofGetCurrentWindow()->getWindowSize();
  if (guiWindowSize != currentWindowSize) {
    guiWindowSize = currentWindowSize;
    synth.setGuiSize(currentWindowSize);
  }
  synth.drawGui();
}

//--------------------------------------------------------------
void ofApp::exit(){
  audioAnalysisClientPtr->stopRecording();
  audioAnalysisClientPtr->closeStream();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
  if (audioAnalysisClientPtr->keyPressed(key)) return;
  if (audioDataPlotsPtr->keyPressed(key)) return;
  if (synth.keyPressed(key)) return;
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
