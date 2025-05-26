#include "ofApp.h"

using namespace ofxMarkSynth;

//--------------------------------------------------------------
ModPtrs ofApp::createMods() {
  auto mods = ModPtrs {};
  
  // Audio and palette
  auto audioDataSourceModPtr = addMod<AudioDataSourceMod>(mods, "Audio Points", {
    {"MinPitch", "50.0"},
    {"MaxPitch", "2500.0"}
  }, audioDataProcessorPtr);
  
  auto audioPaletteModPtr = addMod<SomPaletteMod>(mods, "Palette Creator", {});
  audioDataSourceModPtr->addSink(AudioDataSourceMod::SOURCE_SPECTRAL_POINTS,
                                 audioPaletteModPtr,
                                 SomPaletteMod::SINK_VEC3);
  
  auto clusterModPtr = addMod<ClusterMod>(mods, "Clusters", {});
  audioDataSourceModPtr->addSink(AudioDataSourceMod::SOURCE_POLAR_PITCH_RMS_POINTS,
                                 clusterModPtr,
                                 ClusterMod::SINK_VEC2);
  
  // Fluid
  auto radiiModPtr = addMod<RandomFloatSourceMod>(mods, "Fluid Points Radii", {
    {"CreatedPerUpdate", "0.05"},
    {"Min", "0.005"},
    {"Max", "0.05"}
  }, std::pair<float, float>{0.0, 0.1}, std::pair<float, float>{0.0, 0.1});
  
  auto drawPointsModPtr = addMod<SoftCircleMod>(mods, "Draw Fluid Points", {
    {"ColorMultiplier", "0.1"}
  });
  radiiModPtr->addSink(RandomFloatSourceMod::SOURCE_FLOAT, drawPointsModPtr, DrawPointsMod::SINK_POINT_RADIUS);
  audioPaletteModPtr->addSink(SomPaletteMod::SOURCE_RANDOM_VEC4, drawPointsModPtr, DrawPointsMod::SINK_POINT_COLOR);
  clusterModPtr->addSink(ClusterMod::SOURCE_VEC2, drawPointsModPtr, DrawPointsMod::SINK_POINTS);
  
  auto fluidModPtr = addMod<FluidMod>(mods, "Fluid", {
    {"dt", "0.002"},
    {"value:dissipation", "0.999"},
    {"dt", "0.002"},
    {"vorticity", "20.0"}
  });
  
  drawPointsModPtr->receive(DrawPointsMod::SINK_FBO, fluidFboPtr);
  fluidModPtr->receive(FluidMod::SINK_VALUES_FBO, fluidFboPtr);
  fluidModPtr->receive(FluidMod::SINK_VELOCITIES_FBO, fluidVelocitiesFboPtr);
  
  { // Cluster radial impulses
    auto fluidRadialImpulseModPtr = addMod<FluidRadialImpulseMod>(mods, "Cluster Impulses", {
      {"ImpulseRadius", "0.02"},
      {"ImpulseStrength", "0.05"}
    });
    clusterModPtr->addSink(ClusterMod::SOURCE_VEC2, fluidRadialImpulseModPtr, FluidRadialImpulseMod::SINK_POINTS);
    fluidRadialImpulseModPtr->receive(FluidRadialImpulseMod::SINK_FBO, fluidVelocitiesFboPtr);
  }
  
  { // Raw data points into fluid
    auto drawPointsModPtr = addMod<SoftCircleMod>(mods, "Fluid Raw Points", {
      {"Radius", "0.01"},
      {"ColorMultiplier", "0.2"}
    });
    audioPaletteModPtr->addSink(SomPaletteMod::SOURCE_RANDOM_VEC4, drawPointsModPtr, DrawPointsMod::SINK_POINT_COLOR);
    audioDataSourceModPtr->addSink(AudioDataSourceMod::SOURCE_POLAR_PITCH_RMS_POINTS, drawPointsModPtr, DrawPointsMod::SINK_POINTS);
    drawPointsModPtr->receive(DrawPointsMod::SINK_FBO, fluidFboPtr);
  }
  
  { // Raw data points
    auto drawPointsModPtr = addMod<DrawPointsMod>(mods, "Draw Raw Points", {
      {"PointRadius", "0.004"}
    });
    audioPaletteModPtr->addSink(SomPaletteMod::SOURCE_RANDOM_VEC4, drawPointsModPtr, DrawPointsMod::SINK_POINT_COLOR);
    audioDataSourceModPtr->addSink(AudioDataSourceMod::SOURCE_POLAR_PITCH_RMS_POINTS, drawPointsModPtr, DrawPointsMod::SINK_POINTS);
    
    auto translateModPtr = addMod<TranslateMod>(mods, "Translate Raw Points", {
      {"Translate By", "0.0, 0.0002"}
    });
    drawPointsModPtr->addSink(DrawPointsMod::SOURCE_FBO, translateModPtr, TranslateMod::SINK_FBO);
    
    auto multiplyModPtr = addMod<MultiplyMod>(mods, "Fade Raw Points", {
      {"Multiply By", "1.0, 1.0, 1.0, 0.9"}
    });
    translateModPtr->addSink(TranslateMod::SOURCE_FBO, multiplyModPtr, MultiplyMod::SINK_FBO);
    drawPointsModPtr->receive(DrawPointsMod::SINK_FBO, rawPointsFboPtr);
  }
  
  { // Collage layer (onto the fluid FBO)
    auto pixelSnapshotModPtr = addMod<PixelSnapshotMod>(mods, "Fluid Snapshot", {});
    pixelSnapshotModPtr->receive(PixelSnapshotMod::SINK_FBO, fluidFboPtr);
    
    auto pathModPtr = addMod<PathMod>(mods, "Collage Path", {
      {"MaxVertices", "7"},
      {"VertexProximity", "0.02"}
    });
    audioDataSourceModPtr->addSink(AudioDataSourceMod::SOURCE_POLAR_PITCH_RMS_POINTS,
                                   pathModPtr,
                                   PathMod::SINK_VEC2);
    
//    auto fluidCollageModPtr = addMod<CollageMod>(mods, "Fluid Collage", {});
//    pixelSnapshotModPtr->addSink(PixelSnapshotMod::SOURCE_PIXELS,
//                                 fluidCollageModPtr,
//                                 CollageMod::SINK_PIXELS);
//    pathModPtr->addSink(PathMod::SOURCE_PATH,
//                        fluidCollageModPtr,
//                        CollageMod::SINK_PATH);
//    audioPaletteModPtr->addSink(SomPaletteMod::SOURCE_RANDOM_VEC4,
//                                fluidCollageModPtr,
//                                CollageMod::SINK_COLOR);
//    fluidCollageModPtr->receive(CollageMod::SINK_FBO, fluidFboPtr);
    
    auto collageModPtr = addMod<CollageMod>(mods, "Collage", {});
    pixelSnapshotModPtr->addSink(PixelSnapshotMod::SOURCE_PIXELS,
                                 collageModPtr,
                                 CollageMod::SINK_PIXELS);
    pathModPtr->addSink(PathMod::SOURCE_PATH,
                        collageModPtr,
                        CollageMod::SINK_PATH);
    audioPaletteModPtr->addSink(SomPaletteMod::SOURCE_RANDOM_VEC4,
                                collageModPtr,
                                CollageMod::SINK_COLOR);
    auto multiplyModPtr = addMod<MultiplyMod>(mods, "Fade Collage", {
      {"Multiply By", "1.0, 1.0, 1.0, 0.95"}
    });
    collageModPtr->addSink(CollageMod::SOURCE_FBO, multiplyModPtr, MultiplyMod::SINK_FBO);
    collageModPtr->receive(CollageMod::SINK_FBO, fboCollagePtr);
  }
  
  { // DividedArea
    auto dividedAreaModPtr = addMod<DividedAreaMod>(mods, "Divided Area", {});
    clusterModPtr->addSink(ClusterMod::SOURCE_VEC2,
                           dividedAreaModPtr,
                           DividedAreaMod::SINK_MAJOR_ANCHORS);
    audioDataSourceModPtr->addSink(AudioDataSourceMod::SOURCE_POLAR_PITCH_RMS_POINTS, dividedAreaModPtr, DividedAreaMod::SINK_MINOR_ANCHORS);
    
    auto multiplyModPtr = addMod<MultiplyMod>(mods, "Fade Unconstrained Lines", {
      {"Multiply By", "1.0, 1.0, 1.0, 0.8"}
    });
    dividedAreaModPtr->addSink(DrawPointsMod::SOURCE_FBO_2, // Fade unconstrained lines
                               multiplyModPtr,
                               MultiplyMod::SINK_FBO);
    
    dividedAreaModPtr->receive(DividedAreaMod::SINK_FBO, fboPtrMinorLinesPtr);
    dividedAreaModPtr->receive(DividedAreaMod::SINK_FBO_2, fboPtrMajorLinesPtr);
  }
  
  { // Sandlines
    auto sandLineModPtr = addMod<SandLineMod>(mods, "Sand lines", {
      {"PointRadius", "1.0"},
      {"Density", "0.05"}
    });
    audioPaletteModPtr->addSink(SomPaletteMod::SOURCE_RANDOM_VEC4, sandLineModPtr, SandLineMod::SINK_POINT_COLOR);
    clusterModPtr->addSink(ClusterMod::SOURCE_VEC2,
                           sandLineModPtr,
                           SandLineMod::SINK_POINTS);

    auto multiplyModPtr = addMod<MultiplyMod>(mods, "Fade Sand Lines", {
      {"Multiply By", "1.0, 1.0, 1.0, 0.995"}
    });
    sandLineModPtr->addSink(SandLineMod::SOURCE_FBO,
                            multiplyModPtr,
                            MultiplyMod::SINK_FBO);
    
    sandLineModPtr->receive(CollageMod::SINK_FBO, fboSandlinesPtr);
  }
  
  return mods;
}

FboConfigPtrs ofApp::createFboConfigs() {
  FboConfigPtrs fbos;
  
  auto fboConfigPtrFluidValues = std::make_shared<FboConfig>(fluidFboPtr);
  fbos.emplace_back(fboConfigPtrFluidValues);
  
  auto fboConfigPtrSandlines = std::make_shared<FboConfig>(fboSandlinesPtr);
  fbos.emplace_back(fboConfigPtrSandlines);
  
  auto fboConfigPtrMinorLines = std::make_shared<FboConfig>(fboPtrMinorLinesPtr);
  fbos.emplace_back(fboConfigPtrMinorLines);
  
  auto fboConfigPtrRawPoints = std::make_shared<FboConfig>(rawPointsFboPtr);
  fbos.emplace_back(fboConfigPtrRawPoints);
  
  auto fboConfigPtrCollage = std::make_shared<FboConfig>(fboCollagePtr);
  fbos.emplace_back(fboConfigPtrCollage);
  
  auto fboConfigPtrMajorLines = std::make_shared<FboConfig>(fboPtrMajorLinesPtr);
  fbos.emplace_back(fboConfigPtrMajorLines);
  
  return fbos;
}

// TODO: find a home for this util
void minimizeAllGuiGroupsRecursive(ofxGuiGroup& guiGroup) {
  for (int i = 0; i < guiGroup.getNumControls(); ++i) {
    auto control = guiGroup.getControl(i);
    if (auto childGuiGroup = dynamic_cast<ofxGuiGroup*>(control)) {
      childGuiGroup->minimize();
      minimizeAllGuiGroupsRecursive(*childGuiGroup);
    }
  }
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
  
  allocateFbo(fluidFboPtr, ofGetWindowSize(), GL_RGBA32F, GL_REPEAT);
  fluidFboPtr->getSource().clearColorBuffer(ofFloatColor(0.0, 0.0, 0.0, 0.0));
  
  allocateFbo(fluidVelocitiesFboPtr, ofGetWindowSize(), GL_RGB32F, GL_REPEAT);
  fluidVelocitiesFboPtr->getSource().clearColorBuffer(ofFloatColor(0.0, 0.0, 0.0));
  
  allocateFbo(rawPointsFboPtr, ofGetWindowSize(), GL_RGBA32F, GL_REPEAT);
  rawPointsFboPtr->getSource().clearColorBuffer(ofFloatColor(0.0, 0.0, 0.0, 0.0));
  
  allocateFbo(fboPtrMinorLinesPtr, ofGetWindowSize(), GL_RGBA8);
  fboPtrMinorLinesPtr->getSource().clearColorBuffer(ofFloatColor(0.0, 0.0, 0.0, 0.0));
  
  allocateFbo(fboPtrMajorLinesPtr, ofGetWindowSize(), GL_RGBA32F);
  fboPtrMajorLinesPtr->getSource().clearColorBuffer(ofFloatColor(0.0, 0.0, 0.0, 0.0));
  
  allocateFbo(fboCollagePtr, ofGetWindowSize(), GL_RGBA32F);
  fboCollagePtr->getSource().clearColorBuffer(ofFloatColor(0.0, 0.0, 0.0, 0.0));
  
  allocateFbo(fboSandlinesPtr, ofGetWindowSize(), GL_RGBA32F);
  fboSandlinesPtr->getSource().clearColorBuffer(ofFloatColor(0.0, 0.0, 0.0, 0.0));
  
  synth.configure(createMods(), createFboConfigs(), ofGetWindowSize());
  
  parameters.add(synth.getParameterGroup("Synth"));
  gui.setup(parameters);
  minimizeAllGuiGroupsRecursive(gui.getGroup("Synth"));
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
