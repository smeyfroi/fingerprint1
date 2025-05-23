#pragma once

#include "ofMain.h"
#include "ofxMarkSynth.h"
#include "ofxGui.h"

class ofApp : public ofBaseApp{
  
public:
  void setup() override;
  void update() override;
  void draw() override;
  void exit() override;
  
  void keyPressed(int key) override;
  void keyReleased(int key) override;
  void mouseMoved(int x, int y ) override;
  void mouseDragged(int x, int y, int button) override;
  void mousePressed(int x, int y, int button) override;
  void mouseReleased(int x, int y, int button) override;
  void mouseScrolled(int x, int y, float scrollX, float scrollY) override;
  void mouseEntered(int x, int y) override;
  void mouseExited(int x, int y) override;
  void windowResized(int w, int h) override;
  void dragEvent(ofDragInfo dragInfo) override;
  void gotMessage(ofMessage msg) override;
  
private:
  ofxMarkSynth::Synth synth;
  ofxMarkSynth::ModPtrs createMods();
  ofxMarkSynth::FboConfigPtrs createFboConfigs();
  ofxMarkSynth::FboPtr fboPtr = std::make_shared<PingPongFbo>();
  ofxMarkSynth::FboPtr fluidVelocitiesFboPtr = std::make_shared<PingPongFbo>();
  
  bool guiVisible { true };
  ofxPanel gui;
  ofParameterGroup parameters; // I think we rely on this declaration coming after the synth to ensure that destructors are done in the right order
  
  // >>> TODO: EXTRACT THESE HELPERS
  ofxMarkSynth::ModPtr modByName(ofxMarkSynth::ModPtrs& modPtrs, std::string name) {
    const auto it = std::find_if(modPtrs.begin(), modPtrs.end(), [&](const auto& modPtr){
      return modPtr->name == name;
    });
    if (it == modPtrs.end()) ofLogNotice() << "Can't find Mod '" << name << "'";
    return *it;
  }
  
  template <typename ModT>
  void addMod(ofxMarkSynth::ModPtrs& modPtrs, const std::string& name, ofxMarkSynth::ModConfig&& modConfig) {
    modPtrs.push_back(std::make_shared<ModT>(name, std::forward<ofxMarkSynth::ModConfig>(modConfig)));
  }
  
  template <typename ModT, typename... Args>
  void addMod(ofxMarkSynth::ModPtrs& modPtrs, const std::string& name, ofxMarkSynth::ModConfig&& modConfig, Args&&... args) {
    modPtrs.push_back(std::make_shared<ModT>(name, std::forward<ofxMarkSynth::ModConfig>(modConfig), std::forward<Args>(args)...));
  }

  void connectMods(ofxMarkSynth::ModPtrs& modPtrs, std::string sourceName, int sourceId, std::string sinkName, int sinkId) {
    ofxMarkSynth::ModPtr sourceModPtr = modByName(modPtrs, sourceName);
    ofxMarkSynth::ModPtr sinkModPtr = modByName(modPtrs, sinkName);
    sourceModPtr->addSink(sourceId, sinkModPtr, sinkId);
  }
  // <<<
  
};
