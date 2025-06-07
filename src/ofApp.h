#pragma once

#include "ofMain.h"
#include "ofxMarkSynth.h"

// ***********************************************
// ***********************************************
const std::filesystem::path rootSourceMaterialPath { "/Users/steve/Documents/music-source-material" };
constexpr bool RECORD_FLOW_VIDEO = false;
constexpr bool RECORD_AUDIO = false;
// ***********************************************
// ***********************************************

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
  std::shared_ptr<ofxAudioAnalysisClient::LocalGistClient> audioAnalysisClientPtr;
  std::shared_ptr<ofxAudioData::Processor> audioDataProcessorPtr;
  std::shared_ptr<ofxAudioData::Plots> audioDataPlotsPtr;
  
  void configSynth1();
  void configSynth2();
  ofxMarkSynth::ModPtrs createMods1();
  ofxMarkSynth::FboConfigPtrs createFboConfigs1();
  ofxMarkSynth::ModPtrs createMods2();
  ofxMarkSynth::FboConfigPtrs createFboConfigs2();

  ofxMarkSynth::Synth synth;
  ofxMarkSynth::FboPtr fluidFboPtr = std::make_shared<PingPongFbo>();
  ofxMarkSynth::FboPtr fluidVelocitiesFboPtr = std::make_shared<PingPongFbo>();
  ofxMarkSynth::FboPtr rawPointsFboPtr = std::make_shared<PingPongFbo>();
  ofxMarkSynth::FboPtr fboPtrMinorLinesPtr = std::make_shared<PingPongFbo>();
  ofxMarkSynth::FboPtr fboPtrMajorLinesPtr = std::make_shared<PingPongFbo>();
  ofxMarkSynth::FboPtr fboCollagePtr = std::make_shared<PingPongFbo>();
  ofxMarkSynth::FboPtr fboSandlinesPtr = std::make_shared<PingPongFbo>();
  ofxMarkSynth::FboPtr fboClusterParticlesPtr = std::make_shared<PingPongFbo>();
  ofxMarkSynth::FboPtr fboMotionParticlesPtr = std::make_shared<PingPongFbo>();

};
