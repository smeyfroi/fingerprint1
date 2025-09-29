#include "ofApp.h"
#include "ofxTimeMeasurements.h"

using namespace ofxMarkSynth;

ModPtr findModPtrByName(const std::vector<ModPtr>& mods, const std::string& name) {
  auto it = std::find_if(mods.begin(), mods.end(), [&name](const ModPtr& modPtr) {
    return modPtr->name == name;
  });
  if (it == mods.end()) {
    ofLogError() << "No mod found with name " << name;
    ofExit();
  }
  return *it;
}

void ofApp::configSynth1(glm::vec2 size) {
  synth = std::make_shared<Synth>("Synth1", ModConfig {}, START_PAUSED);
  auto mods = createMods1();
  auto somPaletteModPtr = findModPtrByName(mods, "Palette Creator");
  auto audioDataSourceModPtr = findModPtrByName(mods, "Audio Source");
  synth->configure(createFboConfigs1(size), std::move(mods), size);
  somPaletteModPtr->connect(SomPaletteMod::SOURCE_DARKEST_VEC4, synth, Synth::SINK_BACKGROUND_COLOR);
  audioDataSourceModPtr->connect(AudioDataSourceMod::SOURCE_ONSET1, synth, Synth::SINK_AUDIO_ONSET);
  audioDataSourceModPtr->connect(AudioDataSourceMod::SOURCE_TIMBRE_CHANGE, synth, Synth::SINK_AUDIO_TIMBRE_CHANGE);
}

void ofApp::configSynth2(glm::vec2 size) {
  synth = std::make_shared<Synth>("Synth2", ModConfig {}, START_PAUSED);
  auto mods = createMods2();
  auto somPaletteModPtr = findModPtrByName(mods, "Palette Creator");
  auto audioDataSourceModPtr = findModPtrByName(mods, "Audio Source");
  synth->configure(createFboConfigs2(size), std::move(mods), size);
  somPaletteModPtr->connect(SomPaletteMod::SOURCE_DARKEST_VEC4, synth, Synth::SINK_BACKGROUND_COLOR);
  audioDataSourceModPtr->connect(AudioDataSourceMod::SOURCE_ONSET1, synth, Synth::SINK_AUDIO_ONSET);
  audioDataSourceModPtr->connect(AudioDataSourceMod::SOURCE_TIMBRE_CHANGE, synth, Synth::SINK_AUDIO_TIMBRE_CHANGE);
}

void ofApp::setup(){
  ofSetBackgroundColor(0);
  ofDisableArbTex();
  ofSetFrameRate(FRAME_RATE);
  glEnable(GL_PROGRAM_POINT_SIZE);

  TIME_SAMPLE_SET_FRAMERATE(FRAME_RATE);
  TIME_SAMPLE_SET_DRAW_LOCATION(TIME_MEASUREMENTS_BOTTOM_RIGHT);
  TIME_SAMPLE_DISABLE(); // ************************************************************************

//      audioAnalysisClientPtr = std::make_shared<ofxAudioAnalysisClient::LocalGistClient>(rootSourceMaterialPath/"belfast/20250208-violin-separate-scale-vibrato-harmonics.wav");
//      audioAnalysisClientPtr = std::make_shared<ofxAudioAnalysisClient::LocalGistClient>(rootSourceMaterialPath/"percussion/Alex Petcu Bell Plates.wav");
//      audioAnalysisClientPtr = std::make_shared<ofxAudioAnalysisClient::LocalGistClient>(rootSourceMaterialPath/"percussion/Alex Petcu Sound Bath.wav");
//  audioAnalysisClientPtr = std::make_shared<ofxAudioAnalysisClient::LocalGistClient>(rootSourceMaterialPath/"belfast/20250208-trombone-melody.wav");
//  audioAnalysisClientPtr = std::make_shared<ofxAudioAnalysisClient::LocalGistClient>(rootSourceMaterialPath/"misc/nightsong.wav");
//  audioAnalysisClientPtr = std::make_shared<ofxAudioAnalysisClient::LocalGistClient>(rootSourceMaterialPath/"misc/treganna.wav");

  auto recordingPath = saveFilePath("audio-recordings");
  std::filesystem::create_directory(recordingPath);
  audioAnalysisClientPtr = std::make_shared<ofxAudioAnalysisClient::LocalGistClient>(MIC_DEVICE_NAME, RECORD_AUDIO, recordingPath);

  audioDataProcessorPtr = std::make_shared<ofxAudioData::Processor>(audioAnalysisClientPtr);
  audioDataProcessorPtr->setDefaultValiditySpecs();
  audioDataPlotsPtr = std::make_shared<ofxAudioData::Plots>(audioDataProcessorPtr);

  glm::vec2 size = { 7200, 7200 };
  configSynth1(size);
//  configSynth2(size);
  
//  ofLogNotice() << "ofApp::setup synth configured"; // error happens between this and update()
}

//--------------------------------------------------------------
void ofApp::update(){
//  ofLogNotice() << "ofApp::update frame " << ofGetFrameNum();  // error happens before this and after setup()
  audioDataProcessorPtr->update();
  synth->update();
}

//--------------------------------------------------------------
void ofApp::draw(){
  synth->draw();
  audioDataPlotsPtr->drawPlots();
}

void ofApp::drawGui(ofEventArgs& args){
  static glm::vec2 guiWindowSize = {0.0, 0.0};
  auto currentWindowSize = ofGetCurrentWindow()->getWindowSize();
  if (guiWindowSize != currentWindowSize) {
    guiWindowSize = currentWindowSize;
    synth->setGuiSize(guiWindowSize);
  }
  synth->drawGui();
}

//--------------------------------------------------------------
void ofApp::exit(){
  audioAnalysisClientPtr->stopRecording();
  audioAnalysisClientPtr->closeStream();
  synth->shutdown();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
  if (audioAnalysisClientPtr->keyPressed(key)) return;
  if (audioDataPlotsPtr->keyPressed(key)) return;
  if (synth->keyPressed(key)) return;
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
