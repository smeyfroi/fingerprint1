//
//  ofAppSynth2.cpp
//  fingerprint1
//
//  Created by Steve Meyfroidt on 06/06/2025.
//

#include "ofApp.h"

constexpr int VIDEO_DEVICE_ID = 2; // **********************************************************

using namespace ofxMarkSynth;

ModPtrs ofApp::createMods2() {
  auto mods = ModPtrs {};
  
  // Audio, palette from raw spectral points, clusters from raw pitch/RMS points
  auto audioDataSourceModPtr = addMod<AudioDataSourceMod>(mods, "Audio Points", {
    {"MinPitch", "50.0"},
    {"MaxPitch", "1500.0"},
    {"MinRms", "0.005"},
    {"MaxRms", "0.08"}
  }, audioDataProcessorPtr);
  
  auto audioPaletteModPtr = addMod<SomPaletteMod>(mods, "Palette Creator", {});
  audioDataSourceModPtr->addSink(AudioDataSourceMod::SOURCE_SPECTRAL_POINTS, audioPaletteModPtr, SomPaletteMod::SINK_VEC3);
  
  auto clusterModPtr = addMod<ClusterMod>(mods, "Clusters", {
    {"maxSourcePoints", "600"},
    {"clusters", "7"}
  });
  audioDataSourceModPtr->addSink(AudioDataSourceMod::SOURCE_PITCH_RMS_POINTS, clusterModPtr, ClusterMod::SINK_VEC2);
//  audioDataSourceModPtr->addSink(AudioDataSourceMod::SOURCE_POLAR_PITCH_RMS_POINTS, clusterModPtr, ClusterMod::SINK_VEC2);
  
  { // Raw data points
    auto drawPointsModPtr = addMod<DrawPointsMod>(mods, "Draw Raw Points", {
      {"Radius", "0.002"},
      {"RadiusVarianceScale", "0.014"},
      {"ColorMultiplier", "1.0"}
    });
    audioPaletteModPtr->addSink(SomPaletteMod::SOURCE_RANDOM_VEC4, drawPointsModPtr, DrawPointsMod::SINK_POINT_COLOR);
    audioDataSourceModPtr->addSink(AudioDataSourceMod::SOURCE_PITCH_RMS_POINTS, drawPointsModPtr, DrawPointsMod::SINK_POINTS);
//    audioDataSourceModPtr->addSink(AudioDataSourceMod::SOURCE_POLAR_PITCH_RMS_POINTS, drawPointsModPtr, DrawPointsMod::SINK_POINTS);
    audioDataSourceModPtr->addSink(AudioDataSourceMod::SOURCE_SPECTRAL_CREST_SCALAR, drawPointsModPtr, DrawPointsMod::SINK_POINT_RADIUS_VARIANCE);

    auto fadeModPtr = addMod<FadeMod>(mods, "Fade Raw Points", {
      {"Translation", "0.0, 0.0015"},
      {"Translation Alpha", "1.0"},
      {"Fade Amount", "0.0001"},
    });
    drawPointsModPtr->addSink(DrawPointsMod::SOURCE_FBO, fadeModPtr, TranslateMod::SINK_FBO);

//    auto translateModPtr = addMod<TranslateMod>(mods, "Translate Raw Points", {
//      {"Translate By", "0.0, 0.003"}
//    });
//    drawPointsModPtr->addSink(DrawPointsMod::SOURCE_FBO, translateModPtr, TranslateMod::SINK_FBO);
//    
//    auto multiplyModPtr = addMod<MultiplyMod>(mods, "Fade Raw Points", {
//      {"Multiply By", "0.98"}
//    });
//    translateModPtr->addSink(TranslateMod::SOURCE_FBO, multiplyModPtr, MultiplyMod::SINK_FBO);

    drawPointsModPtr->receive(DrawPointsMod::SINK_FBO, rawPointsFboPtr);
  }
  
  { // DividedArea
    auto dividedAreaModPtr = addMod<DividedAreaMod>(mods, "Divided Area", {
      {"Strategy", "1"},
      {"Angle", "0"},
    });
    clusterModPtr->addSink(ClusterMod::SOURCE_VEC2, dividedAreaModPtr, DividedAreaMod::SINK_MAJOR_ANCHORS);
    audioDataSourceModPtr->addSink(AudioDataSourceMod::SOURCE_POLAR_PITCH_RMS_POINTS, dividedAreaModPtr, DividedAreaMod::SINK_MINOR_ANCHORS);
    
    dividedAreaModPtr->receive(DividedAreaMod::SINK_FBO, fboPtrMinorLinesPtr);
    dividedAreaModPtr->receive(DividedAreaMod::SINK_FBO_2, fboPtrMajorLinesPtr);
  }
  
  { // Sandlines
    auto sandLineModPtr = addMod<SandLineMod>(mods, "Sand lines", {
      {"PointRadius", "5.0"},
      {"Density", "0.2"}
    });
    audioPaletteModPtr->addSink(SomPaletteMod::SOURCE_RANDOM_DARK_VEC4, sandLineModPtr, SandLineMod::SINK_POINT_COLOR);
    clusterModPtr->addSink(ClusterMod::SOURCE_VEC2, sandLineModPtr, SandLineMod::SINK_POINTS);

    auto multiplyModPtr = addMod<MultiplyMod>(mods, "Fade Sand Lines", {
      {"Multiply By", "0.95"}
    });
    sandLineModPtr->addSink(SandLineMod::SOURCE_FBO, multiplyModPtr, MultiplyMod::SINK_FBO);
    
    sandLineModPtr->receive(SandLineMod::SINK_FBO, fboSandlinesPtr);
  }
  
  auto videoFlowModPtr = addMod<VideoFlowSourceMod>(mods, "Video flow", {
    { "SamplesPerUpdate", "0.06" },
    { "velocityScale", "4.0" }
  }, VIDEO_DEVICE_ID, glm::vec2 { 640, 480 }, RECORD_FLOW_VIDEO, saveFilePath("video-recordings"));
//  auto videoFlowModPtr = addMod<VideoFlowSourceMod>(mods, "Video flow", {}, rootSourceMaterialPath/"trombone-trimmed.mov", true);
  
  { // Motion particles
    auto particleSetModPtr = addMod<ParticleSetMod>(mods, "Motion Particles", {
      { "strategy", "0" }, // POINTS, CONNECTIONS, BOTH
      { "maxParticles", "1000" },
      { "maxParticleAge", "100" },
      { "particleVelocityDamping", "0.995" },
      { "particleAttraction", "-0.01" },
      { "particleAttractionRadius", "0.2" },
      { "particleConnectionRadius", "0.04" },
      { "particleDrawRadius", "8.0" },
      { "colourMultiplier", "1.0" },
      { "forceScale", "0.05" },
      { "Spin", "0.001" }
    });
    videoFlowModPtr->addSink(VideoFlowSourceMod::SOURCE_VEC4, particleSetModPtr, ParticleSetMod::SINK_POINT_VELOCITIES);
    audioPaletteModPtr->addSink(SomPaletteMod::SOURCE_RANDOM_LIGHT_VEC4, particleSetModPtr, DrawPointsMod::SINK_POINT_COLOR);

    auto multiplyModPtr = addMod<MultiplyMod>(mods, "Fade Particles", {
      {"Multiply By", "0.99"}
    });
    particleSetModPtr->addSink(ParticleSetMod::SOURCE_FBO, multiplyModPtr, MultiplyMod::SINK_FBO);

    particleSetModPtr->receive(ParticleSetMod::SINK_FBO, fboMotionParticlesPtr);
  }

  { // Cluster particles
    auto particleSetModPtr = addMod<ParticleSetMod>(mods, "Cluster Particles", {
      {"strategy", "1"}, // POINTS, CONNECTIONS, BOTH
      {"maxParticles", "300"},
      {"maxParticleAge", "50"},
      {"particleVelocityDamping", "0.999"},
      {"particleAttraction", "-0.1"},
      {"particleAttractionRadius", "0.15"},
      {"particleConnectionRadius", "0.3"},
      {"particleDrawRadius", "0.002"},
      {"colourMultiplier", "0.4"},
      {"forceScale", "0.15"},
      {"Spin", "-0.02"}
    });
    clusterModPtr->addSink(ClusterMod::SOURCE_VEC2, particleSetModPtr, ParticleSetMod::SINK_POINTS);
    audioPaletteModPtr->addSink(SomPaletteMod::SOURCE_RANDOM_DARK_VEC4, particleSetModPtr, ParticleSetMod::SINK_COLOR);

    auto multiplyModPtr = addMod<MultiplyMod>(mods, "Fade Cluster Particles", {
      {"Multiply By", "0.98"}
    });
    particleSetModPtr->addSink(ParticleSetMod::SOURCE_FBO, multiplyModPtr, MultiplyMod::SINK_FBO);

    particleSetModPtr->receive(ParticleSetMod::SINK_FBO, fboClusterParticlesPtr);
  }

  { // Collage layer from raw pitch/RMS points and the cluster particles FBO pixels
    auto pixelSnapshotModPtr = addMod<PixelSnapshotMod>(mods, "Pixel Snapshot", {});
    pixelSnapshotModPtr->receive(PixelSnapshotMod::SINK_FBO, rawPointsFboPtr);
    
    auto pathModPtr = addMod<PathMod>(mods, "Collage Path", {
      {"Strategy", "0"},
      {"MaxVertices", "7"},
      {"VertexProximity", "0.06"}
    });
    audioDataSourceModPtr->addSink(AudioDataSourceMod::SOURCE_POLAR_PITCH_RMS_POINTS, pathModPtr, PathMod::SINK_VEC2);
    
    auto collageModPtr = addMod<CollageMod>(mods, "Collage", {
      {"Strength", "1.0"}
    });
    pixelSnapshotModPtr->addSink(PixelSnapshotMod::SOURCE_PIXELS, collageModPtr, CollageMod::SINK_PIXELS);
    pathModPtr->addSink(PathMod::SOURCE_PATH, collageModPtr, CollageMod::SINK_PATH);
    audioPaletteModPtr->addSink(SomPaletteMod::SOURCE_RANDOM_VEC4, collageModPtr, CollageMod::SINK_COLOR);

    auto multiplyModPtr = addMod<MultiplyMod>(mods, "Fade Collage", {
      {"Multiply By", "0.97"}
    });
    collageModPtr->addSink(CollageMod::SOURCE_FBO, multiplyModPtr, MultiplyMod::SINK_FBO);
    collageModPtr->receive(CollageMod::SINK_FBO, fboCollagePtr);
  }

  return mods;
}

FboConfigPtrs ofApp::createFboConfigs2(glm::vec2 size) {
  // Used by fluid sim but not drawn
//  allocateFbo(fluidVelocitiesFboPtr, ofGetWindowSize(), GL_RGB32F, GL_REPEAT);
//  fluidVelocitiesFboPtr->getSource().clearColorBuffer({ 0.0, 0.0, 0.0 });
//  fluidVelocitiesFboPtr->getSource().begin();
//  ofEnableBlendMode(OF_BLENDMODE_DISABLED);
//  ofSetColor(ofFloatColor { 0.0, 0.0, 0.0 });
//  ofFill();
//  ofDrawRectangle(0.0, 0.0, fluidVelocitiesFboPtr->getSource().getWidth(), fluidVelocitiesFboPtr->getSource().getHeight());
//  fluidVelocitiesFboPtr->getSource().end();
  
  FboConfigPtrs fboConfigPtrs;
  const ofFloatColor backgroundColor { 0.0, 0.0, 0.0, 0.0 };
//  addFboConfigPtr(fboConfigPtrs, "fluid", fluidFboPtr, fboSize, GL_RGBA32F, GL_REPEAT, backgroundColor, false, OF_BLENDMODE_ALPHA);
  addFboConfigPtr(fboConfigPtrs, "sandlines", fboSandlinesPtr, size, GL_RGBA32F, GL_CLAMP_TO_EDGE, backgroundColor, false, OF_BLENDMODE_ADD, false);
  addFboConfigPtr(fboConfigPtrs, "motion particles", fboMotionParticlesPtr, size, GL_RGBA32F, GL_CLAMP_TO_EDGE, backgroundColor, false, OF_BLENDMODE_ALPHA, false);
  addFboConfigPtr(fboConfigPtrs, "collage", fboCollagePtr, size, GL_RGBA32F, GL_CLAMP_TO_EDGE, backgroundColor, false, OF_BLENDMODE_ALPHA, false);
  addFboConfigPtr(fboConfigPtrs, "raw points", rawPointsFboPtr, size, FLOAT_A_MODE, GL_REPEAT, backgroundColor, false, OF_BLENDMODE_ALPHA, false);
  addFboConfigPtr(fboConfigPtrs, "cluster particles", fboClusterParticlesPtr, size, GL_RGBA32F, GL_CLAMP_TO_EDGE, backgroundColor, false, OF_BLENDMODE_ALPHA, false);
  addFboConfigPtr(fboConfigPtrs, "minor lines", fboPtrMinorLinesPtr, size, GL_RGBA8, GL_CLAMP_TO_EDGE, backgroundColor, true, OF_BLENDMODE_ALPHA, false);
  addFboConfigPtr(fboConfigPtrs, "major lines", fboPtrMajorLinesPtr, size, GL_RGBA32F, GL_CLAMP_TO_EDGE, backgroundColor, true, OF_BLENDMODE_ALPHA, false);
  return fboConfigPtrs;
}
