#include "ofApp.h"
#include "ofxTimeMeasurements.h"

using namespace ofxMarkSynth;

void ofApp::configSynth1(glm::vec2 size) {
  synth.configure(createFboConfigs1(size), createMods1(), size);
  synth.name = "Synth1";
}

void ofApp::configSynth2(glm::vec2 size) {
  synth.configure(createFboConfigs2(size), createMods2(), size);
  synth.name = "Synth2";
}

void ofApp::setup(){
  ofSetBackgroundColor(0);
  ofDisableArbTex();
  ofSetFrameRate(20);
  
  TIME_SAMPLE_SET_FRAMERATE(20);
  TIME_SAMPLE_SET_DRAW_LOCATION(TIME_MEASUREMENTS_BOTTOM_RIGHT);
  TIME_SAMPLE_DISABLE();
//  ofxTimeMeasurements::instance()->setEnabled(false);

//      audioAnalysisClientPtr = std::make_shared<ofxAudioAnalysisClient::LocalGistClient>(rootSourceMaterialPath/"20250208-violin-separate-scale-vibrato-harmonics.wav");
//      audioAnalysisClientPtr = std::make_shared<ofxAudioAnalysisClient::LocalGistClient>(rootSourceMaterialPath/"Alex Petcu Bell Plates.wav");
  //    audioAnalysisClientPtr = std::make_shared<ofxAudioAnalysisClient::LocalGistClient>(rootSourceMaterialPath/"Alex Petcu Sound Bath.wav");
//  audioAnalysisClientPtr = std::make_shared<ofxAudioAnalysisClient::LocalGistClient>(rootSourceMaterialPath/"20250208-trombone-melody.wav");
//  audioAnalysisClientPtr = std::make_shared<ofxAudioAnalysisClient::LocalGistClient>(rootSourceMaterialPath/"nightsong.wav");
//  audioAnalysisClientPtr = std::make_shared<ofxAudioAnalysisClient::LocalGistClient>(rootSourceMaterialPath/"treganna.wav");

  auto recordingPath = saveFilePath("audio-recordings");
  std::filesystem::create_directory(recordingPath);
//  audioAnalysisClientPtr = std::make_shared<ofxAudioAnalysisClient::LocalGistClient>("Apple Inc.: Steve\325s iPhone Microphone", RECORD_AUDIO, recordingPath);
  audioAnalysisClientPtr = std::make_shared<ofxAudioAnalysisClient::LocalGistClient>("Apple Inc.: MacBook Pro Microphone", RECORD_AUDIO, recordingPath);

  audioDataProcessorPtr = std::make_shared<ofxAudioData::Processor>(audioAnalysisClientPtr);
  audioDataProcessorPtr->setDefaultValiditySpecs();
  audioDataPlotsPtr = std::make_shared<ofxAudioData::Plots>(audioDataProcessorPtr);
  
  glm::vec2 size = { 4800, 4800 };
//  glm::vec2 size = { 7200, 7200 };
//  configSynth1(size);
  configSynth2(size);
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
    guiWindowSize.x = 256;
    synth.setGuiSize(guiWindowSize);
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
