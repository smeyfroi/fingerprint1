#include "ofApp.h"

//--------------------------------------------------------------
ofxMarkSynth::ModPtrs ofApp::createMods() {
  auto mods = ofxMarkSynth::ModPtrs {};

  // Audio and palette
  auto audioDataSourceModPtr = addMod<ofxMarkSynth::AudioDataSourceMod>(mods, "Audio Points", {
    {"MinPitch", "50.0"},
    {"MaxPitch", "2500.0"}
  }, audioDataProcessorPtr);
  
  auto audioPaletteModPtr = addMod<ofxMarkSynth::SomPaletteMod>(mods, "Palette Creator", {});
  audioDataSourceModPtr->addSink(ofxMarkSynth::AudioDataSourceMod::SOURCE_SPECTRAL_POINTS,
                                 audioPaletteModPtr,
                                 ofxMarkSynth::SomPaletteMod::SINK_VEC3);
  
  auto clusterModPtr = addMod<ofxMarkSynth::ClusterMod>(mods, "Clusters", {});
  audioDataSourceModPtr->addSink(ofxMarkSynth::AudioDataSourceMod::SOURCE_POLAR_PITCH_RMS_POINTS,
                                 clusterModPtr,
                                 ofxMarkSynth::ClusterMod::SINK_VEC2);
  
  // Fluid
  auto radiiModPtr = addMod<ofxMarkSynth::RandomFloatSourceMod>(mods, "Fluid Points Radii", {
    {"CreatedPerUpdate", "0.05"},
    {"Min", "0.005"},
    {"Max", "0.05"}
  }, std::pair<float, float>{0.0, 0.1}, std::pair<float, float>{0.0, 0.1});
  
  auto drawPointsModPtr = addMod<ofxMarkSynth::DrawPointsMod>(mods, "Draw Fluid Points", {});
  radiiModPtr->addSink(ofxMarkSynth::RandomFloatSourceMod::SOURCE_FLOAT, drawPointsModPtr, ofxMarkSynth::DrawPointsMod::SINK_POINT_RADIUS);
  audioPaletteModPtr->addSink(ofxMarkSynth::SomPaletteMod::SOURCE_RANDOM_VEC4, drawPointsModPtr, ofxMarkSynth::DrawPointsMod::SINK_POINT_COLOR);
  clusterModPtr->addSink(ofxMarkSynth::ClusterMod::SOURCE_VEC2, drawPointsModPtr, ofxMarkSynth::DrawPointsMod::SINK_POINTS);
  
  auto fluidModPtr = addMod<ofxMarkSynth::FluidMod>(mods, "Fluid", {
    {"dt", "0.005"}
  });
  
  drawPointsModPtr->receive(ofxMarkSynth::DrawPointsMod::SINK_FBO, fluidFboPtr);
  fluidModPtr->receive(ofxMarkSynth::FluidMod::SINK_VALUES_FBO, fluidFboPtr);
  fluidModPtr->receive(ofxMarkSynth::FluidMod::SINK_VELOCITIES_FBO, fluidVelocitiesFboPtr);
  
  { // Cluster radial impulses
    auto fluidRadialImpulseModPtr = addMod<ofxMarkSynth::FluidRadialImpulseMod>(mods, "Cluster Impulses", {
      {"ImpulseRadius", "0.04"},
      {"ImpulseStrength", "0.02"}
    });
    clusterModPtr->addSink(ofxMarkSynth::ClusterMod::SOURCE_VEC2, fluidRadialImpulseModPtr, ofxMarkSynth::FluidRadialImpulseMod::SINK_POINTS);
    fluidRadialImpulseModPtr->receive(ofxMarkSynth::FluidRadialImpulseMod::SINK_FBO, fluidVelocitiesFboPtr);
  }
  
  { // Raw data points into fluid
    auto drawPointsModPtr = addMod<ofxMarkSynth::DrawPointsMod>(mods, "Fluid Raw Points", {
      {"PointRadius", "0.005"}
    });
    audioPaletteModPtr->addSink(ofxMarkSynth::SomPaletteMod::SOURCE_RANDOM_VEC4, drawPointsModPtr, ofxMarkSynth::DrawPointsMod::SINK_POINT_COLOR);
    audioDataSourceModPtr->addSink(ofxMarkSynth::AudioDataSourceMod::SOURCE_POLAR_PITCH_RMS_POINTS, drawPointsModPtr, ofxMarkSynth::DrawPointsMod::SINK_POINTS);
    drawPointsModPtr->receive(ofxMarkSynth::DrawPointsMod::SINK_FBO, fluidFboPtr);
  }
  
  { // Raw data points
    auto drawPointsModPtr = addMod<ofxMarkSynth::DrawPointsMod>(mods, "Draw Raw Points", {
      {"PointRadius", "0.005"}
    });
    audioPaletteModPtr->addSink(ofxMarkSynth::SomPaletteMod::SOURCE_RANDOM_VEC4, drawPointsModPtr, ofxMarkSynth::DrawPointsMod::SINK_POINT_COLOR);
    audioDataSourceModPtr->addSink(ofxMarkSynth::AudioDataSourceMod::SOURCE_POLAR_PITCH_RMS_POINTS, drawPointsModPtr, ofxMarkSynth::DrawPointsMod::SINK_POINTS);
    
    auto translateModPtr = addMod<ofxMarkSynth::TranslateMod>(mods, "Translate Raw Points", {});
    drawPointsModPtr->addSink(ofxMarkSynth::DrawPointsMod::SOURCE_FBO, translateModPtr, ofxMarkSynth::TranslateMod::SINK_FBO);
    
    auto multiplyModPtr = addMod<ofxMarkSynth::MultiplyMod>(mods, "Fade Raw Points", {
      {"Multiply By", "1.0,1.0,1.0,0.9"}
    });
    translateModPtr->addSink(ofxMarkSynth::TranslateMod::SOURCE_FBO, multiplyModPtr, ofxMarkSynth::MultiplyMod::SINK_FBO);
    drawPointsModPtr->receive(ofxMarkSynth::DrawPointsMod::SINK_FBO, rawPointsFboPtr);
  }
  
  { // Collage layer (onto the raw points FBO)
    auto pixelSnapshotModPtr = addMod<ofxMarkSynth::PixelSnapshotMod>(mods, "Fluid Snapshot", {});
    pixelSnapshotModPtr->receive(ofxMarkSynth::PixelSnapshotMod::SINK_FBO, fluidFboPtr);
    
    auto pathModPtr = addMod<ofxMarkSynth::PathMod>(mods, "Collage Path", {});
    audioDataSourceModPtr->addSink(ofxMarkSynth::AudioDataSourceMod::SOURCE_POLAR_PITCH_RMS_POINTS,
                                   pathModPtr,
                                   ofxMarkSynth::PathMod::SINK_VEC2);
    
    auto collageModPtr = addMod<ofxMarkSynth::CollageMod>(mods, "Collage", {});
    pixelSnapshotModPtr->addSink(ofxMarkSynth::PixelSnapshotMod::SOURCE_PIXELS,
                                 collageModPtr,
                                 ofxMarkSynth::CollageMod::SINK_PIXELS);
    pathModPtr->addSink(ofxMarkSynth::PathMod::SOURCE_PATH,
                        collageModPtr,
                        ofxMarkSynth::CollageMod::SINK_PATH);
    audioPaletteModPtr->addSink(ofxMarkSynth::SomPaletteMod::SOURCE_RANDOM_VEC4,
                                collageModPtr,
                                ofxMarkSynth::CollageMod::SINK_COLOR);
    collageModPtr->receive(ofxMarkSynth::CollageMod::SINK_FBO, fluidFboPtr);
  }
  
  return mods;
}

ofxMarkSynth::FboConfigPtrs ofApp::createFboConfigs() {
  ofxMarkSynth::FboConfigPtrs fbos;

  auto fboConfigPtrFluidValues = std::make_shared<ofxMarkSynth::FboConfig>(fluidFboPtr);
  fbos.emplace_back(fboConfigPtrFluidValues);

  auto fboConfigPtrRawPoints = std::make_shared<ofxMarkSynth::FboConfig>(rawPointsFboPtr);
  fbos.emplace_back(fboConfigPtrRawPoints);

  return fbos;
}

void ofApp::setup(){
  ofSetBackgroundColor(0);
  ofDisableArbTex();
  ofSetFrameRate(60);
  
  const std::filesystem::path rootSourceMaterialPath { "/Users/steve/Documents/music-source-material" };
//    audioAnalysisClientPtr = std::make_shared<ofxAudioAnalysisClient::LocalGistClient>(rootSourceMaterialPath/"Alex Petcu Bell Plates.wav");
//    audioAnalysisClientPtr = std::make_shared<ofxAudioAnalysisClient::LocalGistClient>(rootSourceMaterialPath/"Alex Petcu Sound Bath.wav");
//    audioAnalysisClientPtr = std::make_shared<ofxAudioAnalysisClient::LocalGistClient>(rootSourceMaterialPath/"20250208-trombone-melody.wav");
  audioAnalysisClientPtr = std::make_shared<ofxAudioAnalysisClient::LocalGistClient>();
  audioDataProcessorPtr = std::make_shared<ofxAudioData::Processor>(audioAnalysisClientPtr);
  audioDataProcessorPtr->setDefaultValiditySpecs();
  audioDataPlotsPtr = std::make_shared<ofxAudioData::Plots>(audioDataProcessorPtr);
  
  ofxMarkSynth::allocateFbo(fluidFboPtr, ofGetWindowSize(), GL_RGBA32F, GL_REPEAT);
  fluidFboPtr->getSource().clearColorBuffer(ofFloatColor(0.0, 0.0, 0.0, 0.0));
  ofxMarkSynth::allocateFbo(fluidVelocitiesFboPtr, ofGetWindowSize(), GL_RGB32F, GL_REPEAT);
  fluidVelocitiesFboPtr->getSource().clearColorBuffer(ofFloatColor(0.0, 0.0, 0.0));
  ofxMarkSynth::allocateFbo(rawPointsFboPtr, ofGetWindowSize(), GL_RGBA32F, GL_REPEAT);
  rawPointsFboPtr->getSource().clearColorBuffer(ofFloatColor(0.0, 0.0, 0.0, 0.0));

  synth.configure(createMods(), createFboConfigs(), ofGetWindowSize());
  
  parameters.add(synth.getParameterGroup("Synth"));
  gui.setup(parameters);
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
  if (guiVisible) gui.draw();
}

//--------------------------------------------------------------
void ofApp::exit(){
  audioAnalysisClientPtr->closeStream();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
  if (key == OF_KEY_TAB) { guiVisible = not guiVisible; return; }
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
