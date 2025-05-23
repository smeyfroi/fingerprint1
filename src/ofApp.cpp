#include "ofApp.h"

//--------------------------------------------------------------
ofxMarkSynth::ModPtrs ofApp::createMods() {
  auto mods = ofxMarkSynth::ModPtrs {};

  addMod<ofxMarkSynth::RandomFloatSourceMod>(mods, "Random Radii", {
    {"CreatedPerUpdate", "0.05"},
    {"Min", "0.001"},
    {"Max", "0.05"}
  }, std::pair<float, float>{0.0, 0.1}, std::pair<float, float>{0.0, 0.1});

  addMod<ofxMarkSynth::RandomVecSourceMod>(mods, "Random Points", {
    {"CreatedPerUpdate", "0.4"}
  }, 2);
  addMod<ofxMarkSynth::RandomVecSourceMod>(mods, "Random Colours", {
    {"CreatedPerUpdate", "0.1"}
  }, 4);
  
  addMod<ofxMarkSynth::DrawPointsMod>(mods, "Draw Points", {});
  connectMods(mods, "Random Colours", ofxMarkSynth::RandomVecSourceMod::SOURCE_VEC4, "Draw Points", ofxMarkSynth::DrawPointsMod::SINK_POINT_COLOR);
  connectMods(mods, "Random Radii", ofxMarkSynth::RandomFloatSourceMod::SOURCE_FLOAT, "Draw Points", ofxMarkSynth::DrawPointsMod::SINK_POINT_RADIUS);
  connectMods(mods, "Random Points", ofxMarkSynth::RandomVecSourceMod::SOURCE_VEC2, "Draw Points", ofxMarkSynth::DrawPointsMod::SINK_POINTS);

  addMod<ofxMarkSynth::FluidMod>(mods, "Fluid", {
    {"dt", "0.01"}
  });

  modByName(mods, "Draw Points")->receive(ofxMarkSynth::DrawPointsMod::SINK_FBO, fboPtr);
  modByName(mods, "Fluid")->receive(ofxMarkSynth::FluidMod::SINK_VALUES_FBO, fboPtr);
  modByName(mods, "Fluid")->receive(ofxMarkSynth::FluidMod::SINK_VELOCITIES_FBO, fluidVelocitiesFboPtr);
  
  return mods;
}

ofxMarkSynth::FboConfigPtrs ofApp::createFboConfigs() {
  ofxMarkSynth::FboConfigPtrs fbos;
  auto fboConfigPtrFluidValues = std::make_shared<ofxMarkSynth::FboConfig>(fboPtr, nullptr);
  fbos.emplace_back(fboConfigPtrFluidValues);
  return fbos;
}

void ofApp::setup(){
  ofSetBackgroundColor(0);
  ofDisableArbTex();
  ofSetFrameRate(60);

  fboPtr->allocate(ofGetWindowWidth(), ofGetWindowHeight(), GL_RGBA32F);
  fboPtr->getSource().clearColorBuffer(ofFloatColor(0.0, 0.0, 0.0, 0.0));
  fluidVelocitiesFboPtr->allocate(ofGetWindowWidth(), ofGetWindowHeight(), GL_RGB32F);
  fluidVelocitiesFboPtr->getSource().clearColorBuffer(ofFloatColor(0.0, 0.0, 0.0));
  synth.configure(createMods(), createFboConfigs(), ofGetWindowSize());
  
  parameters.add(synth.getParameterGroup("Synth"));
  gui.setup(parameters);
}

//--------------------------------------------------------------
void ofApp::update(){
  synth.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
  synth.draw();
  if (guiVisible) gui.draw();
}

//--------------------------------------------------------------
void ofApp::exit(){

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
  if (key == OF_KEY_TAB) { guiVisible = not guiVisible; return; }
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
