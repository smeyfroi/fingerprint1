#include "ofApp.h"

using namespace ofxMarkSynth;

const std::filesystem::path rootSourceMaterialPath { "/Users/steve/Documents/music-source-material" };

// ***********************************************
// ***********************************************
constexpr bool RECORD_FLOW_VIDEO = false;
constexpr bool RECORD_AUDIO = false;
// ***********************************************
// ***********************************************

//--------------------------------------------------------------
ModPtrs ofApp::createMods() {
  auto mods = ModPtrs {};
  
  // Audio, palette from raw spectral points, clusters from raw pitch/RMS points
  auto audioDataSourceModPtr = addMod<AudioDataSourceMod>(mods, "Audio Points", {
    {"MinPitch", "50.0"},
    {"MaxPitch", "2500.0"},
    {"minRmsParameter", "0.001"},
    {"maxRmsParameter", "0.04"}
  }, audioDataProcessorPtr);
  
  auto audioPaletteModPtr = addMod<SomPaletteMod>(mods, "Palette Creator", {});
  audioDataSourceModPtr->addSink(AudioDataSourceMod::SOURCE_SPECTRAL_POINTS, audioPaletteModPtr, SomPaletteMod::SINK_VEC3);
  
  auto clusterModPtr = addMod<ClusterMod>(mods, "Clusters", {});
  audioDataSourceModPtr->addSink(AudioDataSourceMod::SOURCE_POLAR_PITCH_RMS_POINTS, clusterModPtr, ClusterMod::SINK_VEC2);
  
  // Fluid simulation
  auto fluidModPtr = addMod<FluidMod>(mods, "Fluid", {
    {"dt", "0.002"},
    {"value:dissipation", "0.999"},
    {"velocity:dissipation", "0.999"},
    {"dt", "0.008"},
    {"vorticity", "20.0"}
  });
  fluidModPtr->receive(FluidMod::SINK_VALUES_FBO, fluidFboPtr);
  fluidModPtr->receive(FluidMod::SINK_VELOCITIES_FBO, fluidVelocitiesFboPtr);

  // Soft circles into fluid values
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
  drawPointsModPtr->receive(DrawPointsMod::SINK_FBO, fluidFboPtr);
  
  { // Radial impulses from clusters
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
  
  { // Collage layer from raw pitch/RMS and the fluid FBO
    auto pixelSnapshotModPtr = addMod<PixelSnapshotMod>(mods, "Fluid Snapshot", {});
    pixelSnapshotModPtr->receive(PixelSnapshotMod::SINK_FBO, fluidFboPtr);
    
    auto pathModPtr = addMod<PathMod>(mods, "Collage Path", {
      {"MaxVertices", "7"},
      {"VertexProximity", "0.02"}
    });
    audioDataSourceModPtr->addSink(AudioDataSourceMod::SOURCE_POLAR_PITCH_RMS_POINTS, pathModPtr, PathMod::SINK_VEC2);
    
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
    pixelSnapshotModPtr->addSink(PixelSnapshotMod::SOURCE_PIXELS, collageModPtr, CollageMod::SINK_PIXELS);
    pathModPtr->addSink(PathMod::SOURCE_PATH, collageModPtr, CollageMod::SINK_PATH);
    audioPaletteModPtr->addSink(SomPaletteMod::SOURCE_RANDOM_VEC4, collageModPtr, CollageMod::SINK_COLOR);

    auto multiplyModPtr = addMod<MultiplyMod>(mods, "Fade Collage", {
      {"Multiply By", "1.0, 1.0, 1.0, 0.99"}
    });
    collageModPtr->addSink(CollageMod::SOURCE_FBO, multiplyModPtr, MultiplyMod::SINK_FBO);
    collageModPtr->receive(CollageMod::SINK_FBO, fboCollagePtr);
  }
  
  { // DividedArea
    auto dividedAreaModPtr = addMod<DividedAreaMod>(mods, "Divided Area", {});
    clusterModPtr->addSink(ClusterMod::SOURCE_VEC2, dividedAreaModPtr, DividedAreaMod::SINK_MAJOR_ANCHORS);
    audioDataSourceModPtr->addSink(AudioDataSourceMod::SOURCE_POLAR_PITCH_RMS_POINTS, dividedAreaModPtr, DividedAreaMod::SINK_MINOR_ANCHORS);
    
    dividedAreaModPtr->receive(DividedAreaMod::SINK_FBO, fboPtrMinorLinesPtr);
    dividedAreaModPtr->receive(DividedAreaMod::SINK_FBO_2, fboPtrMajorLinesPtr);
  }
  
  { // Sandlines
    auto sandLineModPtr = addMod<SandLineMod>(mods, "Sand lines", {
      {"PointRadius", "1.0"},
      {"Density", "0.1"}
    });
    audioPaletteModPtr->addSink(SomPaletteMod::SOURCE_RANDOM_VEC4, sandLineModPtr, SandLineMod::SINK_POINT_COLOR);
    clusterModPtr->addSink(ClusterMod::SOURCE_VEC2, sandLineModPtr, SandLineMod::SINK_POINTS);

    auto multiplyModPtr = addMod<MultiplyMod>(mods, "Fade Sand Lines", {
      {"Multiply By", "1.0, 1.0, 1.0, 0.999"}
    });
    sandLineModPtr->addSink(SandLineMod::SOURCE_FBO, multiplyModPtr, MultiplyMod::SINK_FBO);
    
    sandLineModPtr->receive(SandLineMod::SINK_FBO, fboSandlinesPtr);
  }
  
  auto videoFlowModPtr = addMod<VideoFlowSourceMod>(mods, "Video flow", {}, 0, glm::vec2 { 640, 480 }, RECORD_FLOW_VIDEO, saveFilePath("video-recordings"));
//  auto videoFlowModPtr = addMod<VideoFlowSourceMod>(mods, "Video flow", {}, rootSourceMaterialPath/"trombone-trimmed.mov", true);
  
  { // Motion particles
    auto particleSetModPtr = addMod<ParticleSetMod>(mods, "Motion Particles", {
      { "particleAttraction", "0.05" },
      { "particleAttractionRadius", "0.2" },
      { "particleConnectionRadius", "0.01" },
      { "particleDrawRadius", "0.0005" },
      { "forceScale", "0.01" }
    });
    videoFlowModPtr->addSink(VideoFlowSourceMod::SOURCE_VEC4, particleSetModPtr, ParticleSetMod::SINK_POINT_VELOCITIES);
    audioPaletteModPtr->addSink(SomPaletteMod::SOURCE_RANDOM_VEC4, particleSetModPtr, DrawPointsMod::SINK_POINT_COLOR);

    auto multiplyModPtr = addMod<MultiplyMod>(mods, "Fade Particles", {
      {"Multiply By", "1.0, 1.0, 1.0, 0.97"}
    });
    particleSetModPtr->addSink(ParticleSetMod::SOURCE_FBO, multiplyModPtr, MultiplyMod::SINK_FBO);

    particleSetModPtr->receive(ParticleSetMod::SINK_FBO, fboMotionParticlesPtr);
  }
  
  { // Video flow into fluid
    auto addTextureModPtr = addMod<AddTextureMod>(mods, "Add motion flow", {});
    videoFlowModPtr->addSink(VideoFlowSourceMod::SOURCE_FLOW_PIXELS, addTextureModPtr, AddTextureMod::SINK_ADD_PIXELS);

    addTextureModPtr->receive(AddTextureMod::SINK_TARGET_FBO, fluidVelocitiesFboPtr);
  }
  
  { // Cluster particles
    auto particleSetModPtr = addMod<ParticleSetMod>(mods, "Cluster Particles", {});
    clusterModPtr->addSink(ClusterMod::SOURCE_VEC2, particleSetModPtr, ParticleSetMod::SINK_POINTS);
    audioPaletteModPtr->addSink(SomPaletteMod::SOURCE_RANDOM_VEC4, particleSetModPtr, DrawPointsMod::SINK_POINT_COLOR);

//    auto multiplyModPtr = addMod<MultiplyMod>(mods, "Fade Particles", {
//      {"Multiply By", "1.0, 1.0, 1.0, 0.995"}
//    });
//    particleSetModPtr->addSink(ParticleSetMod::SOURCE_FBO, multiplyModPtr, MultiplyMod::SINK_FBO);

//    particleSetModPtr->receive(ParticleSetMod::SINK_FBO, fluidFboPtr);
    particleSetModPtr->receive(ParticleSetMod::SINK_FBO, fboSandlinesPtr);
  }
  
  return mods;
}

FboConfigPtrs ofApp::createFboConfigs() {
  FboConfigPtrs fboConfigPtrs;
  
  ofFloatColor backgroundColor { 0.0, 1.0, 1.0, 0.0 };
  addFboConfigPtr(fboConfigPtrs, "fluid", fluidFboPtr, ofGetWindowSize(), GL_RGBA32F, GL_REPEAT, backgroundColor, false, OF_BLENDMODE_ALPHA);
  addFboConfigPtr(fboConfigPtrs, "motion particles", fboMotionParticlesPtr, ofGetWindowSize(), GL_RGBA32F, GL_CLAMP_TO_EDGE, backgroundColor, false, OF_BLENDMODE_ALPHA);
  addFboConfigPtr(fboConfigPtrs, "sandlines", fboSandlinesPtr, ofGetWindowSize(), GL_RGBA32F, GL_CLAMP_TO_EDGE, backgroundColor, false, OF_BLENDMODE_ADD);
  addFboConfigPtr(fboConfigPtrs, "minor lines", fboPtrMinorLinesPtr, ofGetWindowSize(), GL_RGBA8, GL_CLAMP_TO_EDGE, backgroundColor, false, OF_BLENDMODE_ALPHA);
  addFboConfigPtr(fboConfigPtrs, "raw points", rawPointsFboPtr, ofGetWindowSize(), GL_RGBA32F, GL_REPEAT, backgroundColor, false, OF_BLENDMODE_ALPHA);
  addFboConfigPtr(fboConfigPtrs, "collage", fboCollagePtr, ofGetWindowSize(), GL_RGBA32F, GL_CLAMP_TO_EDGE, backgroundColor, false, OF_BLENDMODE_ALPHA);
  addFboConfigPtr(fboConfigPtrs, "major lines", fboPtrMajorLinesPtr, ofGetWindowSize(), GL_RGBA32F, GL_CLAMP_TO_EDGE, backgroundColor, true, OF_BLENDMODE_ALPHA);
  
  return fboConfigPtrs;
}

void ofApp::setup(){
  ofSetBackgroundColor(0);
  ofDisableArbTex();
  ofSetFrameRate(30);
  
  //    audioAnalysisClientPtr = std::make_shared<ofxAudioAnalysisClient::LocalGistClient>(rootSourceMaterialPath/"Alex Petcu Bell Plates.wav");
  //    audioAnalysisClientPtr = std::make_shared<ofxAudioAnalysisClient::LocalGistClient>(rootSourceMaterialPath/"Alex Petcu Sound Bath.wav");
  //    audioAnalysisClientPtr = std::make_shared<ofxAudioAnalysisClient::LocalGistClient>(rootSourceMaterialPath/"20250208-trombone-melody.wav");
  auto recordingPath = saveFilePath("audio-recordings");
  std::filesystem::create_directory(recordingPath);
  audioAnalysisClientPtr = std::make_shared<ofxAudioAnalysisClient::LocalGistClient>(RECORD_AUDIO, recordingPath);
  audioDataProcessorPtr = std::make_shared<ofxAudioData::Processor>(audioAnalysisClientPtr);
  audioDataProcessorPtr->setDefaultValiditySpecs();
  audioDataPlotsPtr = std::make_shared<ofxAudioData::Plots>(audioDataProcessorPtr);
  
  allocateFbo(fluidVelocitiesFboPtr, ofGetWindowSize(), GL_RGB32F, GL_REPEAT);
  fluidVelocitiesFboPtr->getSource().clearColorBuffer(ofFloatColor(0.0, 0.0, 0.0));
  
  synth.configure(createFboConfigs(), createMods(), ofGetWindowSize());
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
