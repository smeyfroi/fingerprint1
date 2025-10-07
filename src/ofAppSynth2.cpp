//
//  ofAppSynth2.cpp
//  fingerprint1
//
//  Created by Steve Meyfroidt on 06/06/2025.
//

#include "ofApp.h"

using namespace ofxMarkSynth;

ModPtrs ofApp::createMods2() {
  auto mods = ModPtrs {};
  
  // Audio source
  auto audioDataSourceModPtr = addMod<AudioDataSourceMod>(mods, "Audio Source", {
    {"MinPitch", "50.0"},
    {"MaxPitch", "1500.0"},
    {"MinRms", "0.0005"},
    {"MaxRms", MAX_RMS},
    {"MinComplexSpectralDifference", "200.0"},
    {"MaxComplexSpectralDifference", "1000.0"},
    {"MinSpectralCrest", "20.0"},
    {"MaxSpectralCrest", "350.0"},
    {"MinZeroCrossingRate", "5.0"},
    {"MaxZeroCrossingRate", "15.0"}
  }, MIC_DEVICE_NAME, RECORD_AUDIO, RECORDING_PATH);
  
  // Palette from raw spectral points
  auto audioPaletteModPtr = addMod<SomPaletteMod>(mods, "Palette Creator", {
    {"Iterations", "4000"}
  });

  // Clusters from raw pitch/RMS points
  auto clusterModPtr = addMod<ClusterMod>(mods, "Clusters", {
    {"maxSourcePoints", "600"},
    {"clusters", "7"}
  });

  // Smeared raw data points
  auto drawPointsModPtr = addMod<SoftCircleMod>(mods, "Raw Points", {
    {"Radius", "0.005"}, // min
    {"RadiusVarianceScale", "0.01"}, // scale the variance to some to the min
    {"Softness", "1.0"},
  });
  
  auto smearModPtr = addMod<SmearMod>(mods, "Smear Raw Points", {
    {"Translation", "0.0, 0.0005"},
    {"MixNew", "0.8"},
    {"AlphaMultiplier", "0.999"},
  });

  // Collage layer from raw pitch/RMS and the raw points FBO
  auto pixelSnapshotModPtr = addMod<PixelSnapshotMod>(mods, "Snapshot", {});

  auto pathModPtr = addMod<PathMod>(mods, "Collage Path", {
    {"MaxVertices", "7"},
    {"VertexProximity", "0.05"},
    {"Strategy", "1"} // 0=polypath; 1=bounds; 2=horizontals; 3=convex hull
  });
  
  auto collageModPtr = addMod<CollageMod>(mods, "Collage", {
    {"Strength", "1.0"},
    {"Strategy", "1"}, // 0=tint; 1=add tinted pixels
  });
  
  auto fadeModPtr = addMod<FadeMod>(mods, "Fade Collage", {
    {"Fade Amount", "0.001"}
  });

  // DividedArea
  auto dividedAreaModPtr = addMod<DividedAreaMod>(mods, "Divided Area", {});

  // Sandlines
  auto sandLineModPtr = addMod<SandLineMod>(mods, "Sand lines", {
    {"PointRadius", "1.0"},
    {"Density", "0.15"},
    {"AlphaMultiplier", "1.0"},
    {"StdDevAlong", "0.5"},
    {"StdDevPerpendicular", "0.004"}
  });
  
  // Fluid simulation
  auto fluidModPtr = addMod<FluidMod>(mods, "Fluid", {
    {"dt", "0.007"},
    {"value:dissipation", "0.999"},
    {"velocity:dissipation", "0.998"},
    {"vorticity", "50.0"},
    {"value:iterations", "0.0"},
    {"velocity:iterations", "0.0"},
    {"pressure:iterations", "25.0"}
  });

  // Clusters into fluid values
  auto fluidPointRadiusModPtr = addMod<RandomFloatSourceMod>(mods, "Fluid Cluster Radius", {
    {"CreatedPerUpdate", "0.05"},
    {"Min", "0.005"},
    {"Max", "0.02"}
  }, std::pair<float, float>{0.0, 0.1}, std::pair<float, float>{0.0, 0.1});
  
  auto drawClusterPointsModPtr = addMod<SoftCircleMod>(mods, "Fluid Clusters", {
    {"ColorMultiplier", "0.5"},
    {"AlphaMultiplier", "0.3"},
    {"Softness", "0.3"}
  });

  // Radial fluid impulses from clusters
  auto clusterRadialImpulseModPtr = addMod<FluidRadialImpulseMod>(mods, "Cluster Impulses", {
    {"ImpulseRadius", "0.05"},
    {"ImpulseStrength", "0.02"}
  });

  // Raw data points into fluid
  auto drawRawPointsModPtr = addMod<SoftCircleMod>(mods, "Fluid Raw Points", {
    {"Radius", "0.01"},
    {"ColorMultiplier", "0.4"},
    {"AlphaMultiplier", "0.85"},
    {"Softness", "0.1"}
  });
  
  // Radial fluid impulses from raw points
  auto rawRadialImpulseModPtr = addMod<FluidRadialImpulseMod>(mods, "Raw Point Impulses", {
    {"ImpulseRadius", "0.02"},
    {"ImpulseStrength", "0.06"}
  });
  
  auto videoFlowModPtr = addMod<VideoFlowSourceMod>(mods, "Video flow", {
    { "offset", "2.0" },
    { "threshold", "0.4" },
    { "force", "5.0" },
    { "power", "0.8" }
  }, VIDEO_DEVICE_ID, glm::vec2 { 1280, 720 }, RECORD_VIDEO, saveFilePath("video-recordings"));
//  auto videoFlowModPtr = addMod<VideoFlowSourceMod>(mods, "Video flow", {}, rootSourceMaterialPath/"trombone-trimmed.mov", true);
  
  // Video ParticleField
  auto particleFieldModPtr = addMod<ParticleFieldMod>(mods, "Particle Field", {
    {"velocityDamping", "0.995"},
    {"forceMultiplier", "0.5"},
    {"maxVelocity", "0.0005"},
    {"particleSize", "3.0"}
  }, 0.0, 0.0, 500'000);
  
//  { // Motion particles
//    auto particleSetModPtr = addMod<ParticleSetMod>(mods, "Motion Particles", {
//      { "strategy", "0" }, // POINTS, CONNECTIONS, BOTH
//      { "maxParticles", "1000" },
//      { "maxParticleAge", "100" },
//      { "particleVelocityDamping", "0.995" },
//      { "particleAttraction", "-0.01" },
//      { "particleAttractionRadius", "0.2" },
//      { "particleConnectionRadius", "0.04" },
//      { "particleDrawRadius", "8.0" },
//      { "colourMultiplier", "1.0" },
//      { "forceScale", "0.05" },
//      { "Spin", "0.001" }
//    });
//    videoFlowModPtr->addSink(VideoFlowSourceMod::SOURCE_VEC4, particleSetModPtr, ParticleSetMod::SINK_POINT_VELOCITIES);
//    audioPaletteModPtr->addSink(SomPaletteMod::SOURCE_RANDOM_LIGHT_VEC4, particleSetModPtr, DrawPointsMod::SINK_POINT_COLOR);
//
//    auto multiplyModPtr = addMod<MultiplyMod>(mods, "Fade Particles", {
//      {"Multiply By", "0.99"}
//    });
//    particleSetModPtr->addSink(ParticleSetMod::SOURCE_FBO, multiplyModPtr, MultiplyMod::SINK_FBO);
//
//    particleSetModPtr->receive(ParticleSetMod::SINK_FBO, fboMotionParticlesPtr);
//  }

  audioDataSourceModPtr->connect(AudioDataSourceMod::SOURCE_SPECTRAL_POINTS, audioPaletteModPtr, SomPaletteMod::SINK_VEC3);
  audioDataSourceModPtr->connect(AudioDataSourceMod::SOURCE_POLAR_PITCH_RMS_POINTS, clusterModPtr, ClusterMod::SINK_VEC2);
  audioPaletteModPtr->connect(SomPaletteMod::SOURCE_RANDOM_LIGHT_VEC4, drawPointsModPtr, SoftCircleMod::SINK_POINT_COLOR);
  //    audioDataSourceModPtr->addSink(AudioDataSourceMod::SOURCE_PITCH_RMS_POINTS, drawPointsModPtr, SoftCircleMod::SINK_POINTS);
  audioDataSourceModPtr->connect(AudioDataSourceMod::SOURCE_POLAR_PITCH_RMS_POINTS, drawPointsModPtr, SoftCircleMod::SINK_POINTS);
  audioDataSourceModPtr->connect(AudioDataSourceMod::SOURCE_SPECTRAL_CREST_SCALAR, drawPointsModPtr, SoftCircleMod::SINK_POINT_RADIUS_VARIANCE);
  audioDataSourceModPtr->connect(AudioDataSourceMod::SOURCE_PITCH_SCALAR, drawPointsModPtr, SoftCircleMod::SINK_POINT_COLOR_MULTIPLIER);
  audioDataSourceModPtr->connect(AudioDataSourceMod::SOURCE_POLAR_PITCH_RMS_POINTS, pathModPtr, PathMod::SINK_VEC2);
  clusterModPtr->connect(ClusterMod::SOURCE_VEC2, dividedAreaModPtr, DividedAreaMod::SINK_MAJOR_ANCHORS);
  //    audioDataSourceModPtr->addSink(AudioDataSourceMod::SOURCE_POLAR_PITCH_RMS_POINTS, dividedAreaModPtr, DividedAreaMod::SINK_MINOR_ANCHORS);
  pathModPtr->connect(PathMod::SOURCE_PATH, dividedAreaModPtr, DividedAreaMod::SINK_MINOR_PATH);
  audioPaletteModPtr->connect(SomPaletteMod::SOURCE_RANDOM_LIGHT_VEC4, sandLineModPtr, SandLineMod::SINK_POINT_COLOR);
  clusterModPtr->connect(ClusterMod::SOURCE_VEC2, sandLineModPtr, SandLineMod::SINK_POINTS);
  pixelSnapshotModPtr->connect(PixelSnapshotMod::SOURCE_SNAPSHOT, collageModPtr, CollageMod::SINK_SNAPSHOT_FBO);
  pathModPtr->connect(PathMod::SOURCE_PATH, collageModPtr, CollageMod::SINK_PATH);
  audioPaletteModPtr->connect(SomPaletteMod::SOURCE_RANDOM_LIGHT_VEC4, collageModPtr, CollageMod::SINK_COLOR);
  fluidPointRadiusModPtr->connect(RandomFloatSourceMod::SOURCE_FLOAT, drawClusterPointsModPtr, SoftCircleMod::SINK_POINT_RADIUS);
  audioPaletteModPtr->connect(SomPaletteMod::SOURCE_RANDOM_VEC4, drawClusterPointsModPtr, SoftCircleMod::SINK_POINT_COLOR);
  clusterModPtr->connect(ClusterMod::SOURCE_VEC2, drawClusterPointsModPtr, SoftCircleMod::SINK_POINTS);
  clusterModPtr->connect(ClusterMod::SOURCE_VEC2, clusterRadialImpulseModPtr, FluidRadialImpulseMod::SINK_POINTS);
  audioPaletteModPtr->connect(SomPaletteMod::SOURCE_RANDOM_DARK_VEC4, drawRawPointsModPtr, SoftCircleMod::SINK_POINT_COLOR);
  //    audioDataSourceModPtr->addSink(AudioDataSourceMod::SOURCE_PITCH_RMS_POINTS, drawPointsModPtr, DrawPointsMod::SINK_POINTS);
  audioDataSourceModPtr->connect(AudioDataSourceMod::SOURCE_POLAR_PITCH_RMS_POINTS, drawRawPointsModPtr, SoftCircleMod::SINK_POINTS);
  //    audioDataSourceModPtr->addSink(AudioDataSourceMod::SOURCE_SPECTRAL_CREST_SCALAR, drawPointsModPtr, SoftCircleMod::SINK_POINT_RADIUS_VARIANCE);
  //    audioDataSourceModPtr->addSink(AudioDataSourceMod::SOURCE_PITCH_SCALAR, drawPointsModPtr, SoftCircleMod::SINK_POINT_COLOR_MULTIPLIER);
  //    audioDataSourceModPtr->addSink(AudioDataSourceMod::SOURCE_RMS_SCALAR, drawPointsModPtr, SoftCircleMod::SINK_POINT_SOFTNESS);
  audioDataSourceModPtr->connect(AudioDataSourceMod::SOURCE_POLAR_PITCH_RMS_POINTS, rawRadialImpulseModPtr, FluidRadialImpulseMod::SINK_POINTS);
  videoFlowModPtr->connect(VideoFlowSourceMod::SOURCE_FLOW_FBO, particleFieldModPtr, ParticleFieldMod::SINK_FIELD_1_FBO);
  audioPaletteModPtr->connect(SomPaletteMod::SOURCE_FIELD, particleFieldModPtr, ParticleFieldMod::SINK_FIELD_2_FBO);

  drawPointsModPtr->receive(DrawPointsMod::SINK_FBOPTR, rawPointsFboPtr);
  drawPointsModPtr->receive(SmearMod::SINK_FBOPTR, rawPointsFboPtr);
  dividedAreaModPtr->receive(DividedAreaMod::SINK_FBOPTR_2, fboPtrMinorLinesPtr);
  dividedAreaModPtr->receive(DividedAreaMod::SINK_FBOPTR, fboPtrMajorLinesPtr);
  sandLineModPtr->receive(SandLineMod::SINK_FBOPTR, rawPointsFboPtr);
  fluidModPtr->receive(FluidMod::SINK_VALUES_FBOPTR, fluidFboPtr);
  fluidModPtr->receive(FluidMod::SINK_VELOCITIES_FBOPTR, fluidVelocitiesFboPtr);
  pixelSnapshotModPtr->receive(PixelSnapshotMod::SINK_FBOPTR, fluidFboPtr);
  collageModPtr->receive(CollageMod::SINK_FBOPTR, fboCollagePtr);
  fadeModPtr->receive(FadeMod::SINK_FBOPTR, fboCollagePtr);
  drawClusterPointsModPtr->receive(SoftCircleMod::SINK_FBOPTR, fluidFboPtr);
  clusterRadialImpulseModPtr->receive(FluidRadialImpulseMod::SINK_FBOPTR, fluidVelocitiesFboPtr);
  drawRawPointsModPtr->receive(SoftCircleMod::SINK_FBOPTR, fluidFboPtr);
  rawRadialImpulseModPtr->receive(FluidRadialImpulseMod::SINK_FBOPTR, fluidVelocitiesFboPtr);
  particleFieldModPtr->receive(ParticleFieldMod::SINK_FBOPTR, fboMotionParticlesPtr);

  return mods;
}

FboConfigPtrs ofApp::createFboConfigs2(glm::vec2 size) {
  // Used by fluid sim but not drawn
  allocateFbo(fluidVelocitiesFboPtr, size / 8.0, GL_RGB16F, GL_REPEAT);
  fluidVelocitiesFboPtr->clearFloat(0.0, 0.0, 0.0, 0.0);

  FboConfigPtrs fboConfigPtrs;
  addFboConfigPtr(fboConfigPtrs, "fluid", fluidFboPtr, size / 8.0, GL_RGBA16F, GL_REPEAT, false, OF_BLENDMODE_ALPHA, false, 0);
  addFboConfigPtr(fboConfigPtrs, "major lines", fboPtrMajorLinesPtr, size, GL_RGBA32F, GL_CLAMP_TO_EDGE, true, OF_BLENDMODE_ALPHA, false, 0);
  addFboConfigPtr(fboConfigPtrs, "sandlines", fboSandlinesPtr, size, GL_RGBA32F, GL_CLAMP_TO_EDGE, false, OF_BLENDMODE_ADD, false, 0);
  addFboConfigPtr(fboConfigPtrs, "collage", fboCollagePtr, size, GL_RGBA32F, GL_CLAMP_TO_EDGE, false, OF_BLENDMODE_ALPHA, false, 0);
  addFboConfigPtr(fboConfigPtrs, "raw points", rawPointsFboPtr, size, FLOAT_A_MODE, GL_REPEAT, false, OF_BLENDMODE_ALPHA, false, 0);
  addFboConfigPtr(fboConfigPtrs, "cluster particles", fboClusterParticlesPtr, size, GL_RGBA32F, GL_CLAMP_TO_EDGE, false, OF_BLENDMODE_ALPHA, false, 0);
  addFboConfigPtr(fboConfigPtrs, "motion particles", fboMotionParticlesPtr, size/4.0, GL_RGBA, GL_CLAMP_TO_EDGE, true, OF_BLENDMODE_ADD, false, 0);
  addFboConfigPtr(fboConfigPtrs, "minor lines", fboPtrMinorLinesPtr, size, GL_RGBA8, GL_CLAMP_TO_EDGE, true, OF_BLENDMODE_ALPHA, false, 0);
  return fboConfigPtrs;
}
