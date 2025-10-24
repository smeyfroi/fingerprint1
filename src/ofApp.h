#pragma once

#include "ofMain.h"
#include "ofxMarkSynth.h"
#include "ofxLaunchControllers.h"


// ***********************************************
// ***********************************************
const std::filesystem::path ROOT_SOURCE_MATERIAL_PATH { "/Users/steve/Documents/music-source-material" };
constexpr int VIDEO_DEVICE_ID = 0;
constexpr bool RECORD_VIDEO = false;
constexpr bool RECORD_AUDIO = false;
//const std::string MIC_DEVICE_NAME = "Audient: iD4";
//const std::string MIC_DEVICE_NAME = "Apple Inc.: Steve\325s iPhone Microphone";
const std::string MIC_DEVICE_NAME = "Apple Inc.: MacBook Pro Microphone";
constexpr float FRAME_RATE = 30.0;
const bool START_PAUSED = false; // false for dev
const std::string MAX_RMS = "0.02"; // "0.02"; // "0.11" more likely for live
const std::filesystem::path RECORDING_PATH { "/Users/steve/Documents/recordings" };
//constexpr int SOUND_OUT_DEVICE_ID = 1;
// ***********************************************
// ***********************************************


std::shared_ptr<ofxMarkSynth::Synth> createSynthSoftCircle(glm::vec2 size);
std::shared_ptr<ofxMarkSynth::Synth> createSynth1(glm::vec2 size);
std::shared_ptr<ofxMarkSynth::Synth> createSynth2(glm::vec2 size);


class ofApp : public ofBaseApp{
  
public:
  void setup() override;
  void setupGui();
  void update() override;
  void draw() override;
  void exit() override;
  void drawGui(ofEventArgs& args);

  void keyPressedEvent(ofKeyEventArgs& e) { keyPressed(e.key); } // adapter for ofAddListener
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
  std::shared_ptr<ofxMarkSynth::Synth> synthPtr;
  
  ofxLaunchControlXL lc;
};
