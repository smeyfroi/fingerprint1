//
//  ofAppSynth1.cpp
//  fingerprint1
//
//  Created by Steve Meyfroidt on 06/06/2025.
//

#include "ofApp.h"

using namespace ofxMarkSynth;

ModPtrs ofApp::createMods1() {
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
  audioDataSourceModPtr->addSink(AudioDataSourceMod::SOURCE_POLAR_PITCH_RMS_POINTS, clusterModPtr, ClusterMod::SINK_VEC2);
  
  // Fluid simulation
//  auto fluidModPtr = addMod<FluidMod>(mods, "Fluid", {
//    {"dt", "0.005"},
//    {"value:dissipation", "0.999"},
//    {"velocity:dissipation", "0.998"},
//    {"vorticity", "10.0"}
//  });
//  fluidModPtr->receive(FluidMod::SINK_VALUES_FBO, fluidFboPtr);
//  fluidModPtr->receive(FluidMod::SINK_VELOCITIES_FBO, fluidVelocitiesFboPtr);

  // Soft circles into fluid values
//  auto radiiModPtr = addMod<RandomFloatSourceMod>(mods, "Fluid Points Radii", {
//    {"CreatedPerUpdate", "0.05"},
//    {"Min", "0.005"},
//    {"Max", "0.05"}
//  }, std::pair<float, float>{0.0, 0.1}, std::pair<float, float>{0.0, 0.1});
//  
//  auto drawPointsModPtr = addMod<SoftCircleMod>(mods, "Draw Fluid Points", {
//    {"ColorMultiplier", "0.1"}
//  });
//  radiiModPtr->addSink(RandomFloatSourceMod::SOURCE_FLOAT, drawPointsModPtr, DrawPointsMod::SINK_POINT_RADIUS);
//  audioPaletteModPtr->addSink(SomPaletteMod::SOURCE_RANDOM_VEC4, drawPointsModPtr, DrawPointsMod::SINK_POINT_COLOR);
//  clusterModPtr->addSink(ClusterMod::SOURCE_VEC2, drawPointsModPtr, DrawPointsMod::SINK_POINTS);
//  drawPointsModPtr->receive(DrawPointsMod::SINK_FBO, fluidFboPtr);
//  
//  { // Radial impulses from clusters
//    auto fluidRadialImpulseModPtr = addMod<FluidRadialImpulseMod>(mods, "Cluster Impulses", {
//      {"ImpulseRadius", "0.02"},
//      {"ImpulseStrength", "0.05"}
//    });
//    clusterModPtr->addSink(ClusterMod::SOURCE_VEC2, fluidRadialImpulseModPtr, FluidRadialImpulseMod::SINK_POINTS);
//    fluidRadialImpulseModPtr->receive(FluidRadialImpulseMod::SINK_FBO, fluidVelocitiesFboPtr);
//  }
//  
//  { // Raw data points into fluid
//    auto drawPointsModPtr = addMod<SoftCircleMod>(mods, "Fluid Raw Points", {
//      {"Radius", "0.01"},
//      {"ColorMultiplier", "0.2"}
//    });
//    audioPaletteModPtr->addSink(SomPaletteMod::SOURCE_RANDOM_DARK_VEC4, drawPointsModPtr, DrawPointsMod::SINK_POINT_COLOR);
////    audioDataSourceModPtr->addSink(AudioDataSourceMod::SOURCE_PITCH_RMS_POINTS, drawPointsModPtr, DrawPointsMod::SINK_POINTS);
//    audioDataSourceModPtr->addSink(AudioDataSourceMod::SOURCE_POLAR_PITCH_RMS_POINTS, drawPointsModPtr, DrawPointsMod::SINK_POINTS);
//    drawPointsModPtr->receive(DrawPointsMod::SINK_FBO, fluidFboPtr);
//  }
  
  { // Smeared raw data points
    auto drawPointsModPtr = addMod<DrawPointsMod>(mods, "Draw Raw Points", {
      {"PointRadius", "0.0001"},
      {"PointRadiusVarianceScale", "0.03"},
      {"ColorMultiplier", "0.6"}
    });
    audioPaletteModPtr->addSink(SomPaletteMod::SOURCE_RANDOM_LIGHT_VEC4, drawPointsModPtr, DrawPointsMod::SINK_POINT_COLOR);
//    audioDataSourceModPtr->addSink(AudioDataSourceMod::SOURCE_PITCH_RMS_POINTS, drawPointsModPtr, DrawPointsMod::SINK_POINTS);
    audioDataSourceModPtr->addSink(AudioDataSourceMod::SOURCE_POLAR_PITCH_RMS_POINTS, drawPointsModPtr, DrawPointsMod::SINK_POINTS);
    audioDataSourceModPtr->addSink(AudioDataSourceMod::SOURCE_SPECTRAL_CREST_SCALAR, drawPointsModPtr, DrawPointsMod::SINK_POINT_RADIUS_VARIANCE);
    audioDataSourceModPtr->addSink(AudioDataSourceMod::SOURCE_PITCH_SCALAR, drawPointsModPtr, DrawPointsMod::SINK_POINT_COLOR_MULTIPLIER);

    auto smearModPtr = addMod<SmearMod>(mods, "Smear Raw Points", {
      {"Translation", "0.0, 0.0005"},
      {"Alpha", "0.7"},
    });
    drawPointsModPtr->addSink(DrawPointsMod::SOURCE_FBO, smearModPtr, SmearMod::SINK_FBO);

    drawPointsModPtr->receive(DrawPointsMod::SINK_FBO, rawPointsFboPtr);
  }
  
//  { // Collage layer from raw pitch/RMS and the fluid FBO
//    auto pixelSnapshotModPtr = addMod<PixelSnapshotMod>(mods, "Fluid Snapshot", {});
//    pixelSnapshotModPtr->receive(PixelSnapshotMod::SINK_FBO, fluidFboPtr);
//    
//    auto pathModPtr = addMod<PathMod>(mods, "Collage Path", {
//      {"MaxVertices", "7"},
//      {"VertexProximity", "0.02"}
//    });
//    audioDataSourceModPtr->addSink(AudioDataSourceMod::SOURCE_POLAR_PITCH_RMS_POINTS, pathModPtr, PathMod::SINK_VEC2);
//    
////    auto fluidCollageModPtr = addMod<CollageMod>(mods, "Fluid Collage", {});
////    pixelSnapshotModPtr->addSink(PixelSnapshotMod::SOURCE_PIXELS,
////                                 fluidCollageModPtr,
////                                 CollageMod::SINK_PIXELS);
////    pathModPtr->addSink(PathMod::SOURCE_PATH,
////                        fluidCollageModPtr,
////                        CollageMod::SINK_PATH);
////    audioPaletteModPtr->addSink(SomPaletteMod::SOURCE_RANDOM_VEC4,
////                                fluidCollageModPtr,
////                                CollageMod::SINK_COLOR);
////    fluidCollageModPtr->receive(CollageMod::SINK_FBO, fluidFboPtr);
//    
//    auto collageModPtr = addMod<CollageMod>(mods, "Collage", {
//      {"Strength", "0.4"}
//    });
//    pixelSnapshotModPtr->addSink(PixelSnapshotMod::SOURCE_PIXELS, collageModPtr, CollageMod::SINK_PIXELS);
//    pathModPtr->addSink(PathMod::SOURCE_PATH, collageModPtr, CollageMod::SINK_PATH);
//    audioPaletteModPtr->addSink(SomPaletteMod::SOURCE_RANDOM_VEC4, collageModPtr, CollageMod::SINK_COLOR);
//
//    auto multiplyModPtr = addMod<MultiplyMod>(mods, "Fade Collage", {
//      {"Multiply By", "0.99"}
//    });
//    collageModPtr->addSink(CollageMod::SOURCE_FBO, multiplyModPtr, MultiplyMod::SINK_FBO);
//    collageModPtr->receive(CollageMod::SINK_FBO, fboCollagePtr);
//  }
  
//  { // DividedArea
//    auto dividedAreaModPtr = addMod<DividedAreaMod>(mods, "Divided Area", {});
//    clusterModPtr->addSink(ClusterMod::SOURCE_VEC2, dividedAreaModPtr, DividedAreaMod::SINK_MAJOR_ANCHORS);
//    audioDataSourceModPtr->addSink(AudioDataSourceMod::SOURCE_POLAR_PITCH_RMS_POINTS, dividedAreaModPtr, DividedAreaMod::SINK_MINOR_ANCHORS);
//    
//    dividedAreaModPtr->receive(DividedAreaMod::SINK_FBO_2, fboPtrMinorLinesPtr);
//    dividedAreaModPtr->receive(DividedAreaMod::SINK_FBO, fboPtrMajorLinesPtr);
//  }
  
//  { // Sandlines
//    auto sandLineModPtr = addMod<SandLineMod>(mods, "Sand lines", {
//      {"PointRadius", "1.0"},
//      {"Density", "0.5"}
//    });
//    audioPaletteModPtr->addSink(SomPaletteMod::SOURCE_RANDOM_DARK_VEC4, sandLineModPtr, SandLineMod::SINK_POINT_COLOR);
//    clusterModPtr->addSink(ClusterMod::SOURCE_VEC2, sandLineModPtr, SandLineMod::SINK_POINTS);
//
//    auto multiplyModPtr = addMod<MultiplyMod>(mods, "Fade Sand Lines", {
//      {"Multiply By", "0.995"}
//    });
//    sandLineModPtr->addSink(SandLineMod::SOURCE_FBO, multiplyModPtr, MultiplyMod::SINK_FBO);
//    
////    sandLineModPtr->receive(SandLineMod::SINK_FBO, fboSandlinesPtr);
//    sandLineModPtr->receive(SandLineMod::SINK_FBO, fluidFboPtr);
//  }
  
//  { // Cluster particles
//    auto particleSetModPtr = addMod<ParticleSetMod>(mods, "Cluster Particles", {
//      {"maxParticles", "500"},
//      {"maxParticleAge", "300"},
//      {"colourMultiplier", "0.5"}
//    });
//    clusterModPtr->addSink(ClusterMod::SOURCE_VEC2, particleSetModPtr, ParticleSetMod::SINK_POINTS);
//    audioPaletteModPtr->addSink(SomPaletteMod::SOURCE_RANDOM_LIGHT_VEC4, particleSetModPtr, DrawPointsMod::SINK_POINT_COLOR);
//
//    auto multiplyModPtr = addMod<MultiplyMod>(mods, "Fade Cluster Particles", {
//      {"Multiply By", "0.995"}
//    });
//    particleSetModPtr->addSink(ParticleSetMod::SOURCE_FBO, multiplyModPtr, MultiplyMod::SINK_FBO);
//
////    particleSetModPtr->receive(ParticleSetMod::SINK_FBO, fluidFboPtr);
//    particleSetModPtr->receive(ParticleSetMod::SINK_FBO, fboClusterParticlesPtr);
//  }
  
  return mods;
}

FboConfigPtrs ofApp::createFboConfigs1(glm::vec2 size) {
  // Used by fluid sim but not drawn
  allocateFbo(fluidVelocitiesFboPtr, size / 4.0, GL_RGB32F, GL_REPEAT);
  fluidVelocitiesFboPtr->getSource().clearColorBuffer({ 0.0, 0.0, 0.0 });
//  fluidVelocitiesFboPtr->getSource().begin();
//  ofEnableBlendMode(OF_BLENDMODE_DISABLED);
//  ofSetColor(ofFloatColor { 0.0, 0.0, 0.0 });
//  ofFill();
//  ofDrawRectangle(0.0, 0.0, fluidVelocitiesFboPtr->getSource().getWidth(), fluidVelocitiesFboPtr->getSource().getHeight());
//  fluidVelocitiesFboPtr->getSource().end();
  
  FboConfigPtrs fboConfigPtrs;
  const ofFloatColor backgroundColor { 0.0, 0.0, 0.0, 0.0 };
  addFboConfigPtr(fboConfigPtrs, "fluid", fluidFboPtr, size / 4.0, GL_RGBA32F, GL_REPEAT, backgroundColor, false, OF_BLENDMODE_ALPHA);
  addFboConfigPtr(fboConfigPtrs, "sandlines", fboSandlinesPtr, size, GL_RGBA32F, GL_CLAMP_TO_EDGE, backgroundColor, false, OF_BLENDMODE_ADD);
  addFboConfigPtr(fboConfigPtrs, "minor lines", fboPtrMinorLinesPtr, size, GL_RGBA8, GL_CLAMP_TO_EDGE, backgroundColor, true, OF_BLENDMODE_ALPHA);
  addFboConfigPtr(fboConfigPtrs, "raw points", rawPointsFboPtr, size, GL_RGBA8, GL_REPEAT, backgroundColor, false, OF_BLENDMODE_ALPHA);
  addFboConfigPtr(fboConfigPtrs, "collage", fboCollagePtr, size, GL_RGBA32F, GL_CLAMP_TO_EDGE, backgroundColor, false, OF_BLENDMODE_ALPHA);
  addFboConfigPtr(fboConfigPtrs, "cluster particles", fboClusterParticlesPtr, size, GL_RGBA32F, GL_CLAMP_TO_EDGE, backgroundColor, false, OF_BLENDMODE_ALPHA);
  addFboConfigPtr(fboConfigPtrs, "major lines", fboPtrMajorLinesPtr, size, GL_RGBA32F, GL_CLAMP_TO_EDGE, backgroundColor, true, OF_BLENDMODE_ALPHA);
  return fboConfigPtrs;
}
