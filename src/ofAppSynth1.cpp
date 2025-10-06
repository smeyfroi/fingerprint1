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
  
  // Palette ParticleField
  auto particleFieldModPtr = addMod<ParticleFieldMod>(mods, "Particle Field", {
    {"velocityDamping", "0.994"},
    {"forceMultiplier", "0.2"},
    {"maxVelocity", "0.0001"},
    {"particleSize", "4.0"}
  }, 0.0, 0.0, 500'000);
  
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
  auto radiiModPtr = addMod<RandomFloatSourceMod>(mods, "Fluid Cluster Radius", {
    {"CreatedPerUpdate", "0.05"},
    {"Min", "0.005"},
    {"Max", "0.02"}
  }, std::pair<float, float>{0.0, 0.1}, std::pair<float, float>{0.0, 0.1});
  
  auto drawPointsModPtr = addMod<SoftCircleMod>(mods, "Fluid Clusters", {
    {"ColorMultiplier", "0.5"},
    {"AlphaMultiplier", "0.3"},
    {"Softness", "0.3"}
  });

  // Radial fluid impulses from clusters
  auto fluidRadialImpulseModPtr = addMod<FluidRadialImpulseMod>(mods, "Cluster Impulses", {
    {"ImpulseRadius", "0.05"},
    {"ImpulseStrength", "0.02"}
  });
  clusterModPtr->connect(ClusterMod::SOURCE_VEC2, fluidRadialImpulseModPtr, FluidRadialImpulseMod::SINK_POINTS);

  // Raw data points into fluid
  auto rawFluidPointsModPtr = addMod<SoftCircleMod>(mods, "Fluid Raw Points", {
    {"Radius", "0.01"},
    {"ColorMultiplier", "0.4"},
    {"AlphaMultiplier", "0.6"},
    {"Softness", "0.5"}
  });
    
  // Radial fluid impulses from raw points
  auto rawFluidRadialImpulseModPtr = addMod<FluidRadialImpulseMod>(mods, "Raw Point Impulses", {
    {"ImpulseRadius", "0.025"},
    {"ImpulseStrength", "0.08"}
  });
  
  // Smeared raw data points
  auto smearedPointsModPtr = addMod<SoftCircleMod>(mods, "Raw Points", {
    {"Radius", "0.005"}, // min
    {"RadiusVarianceScale", "0.01"}, // scale the variance to some to the min
    {"Softness", "0.5"},
    {"AlphaMultiplier", "0.9"},
    {"ColorMultiplier", "0.8"}
  });
  
  auto smearModPtr = addMod<SmearMod>(mods, "Smear Raw Points", {
    {"Translation", "0.0, 0.0"},
    {"MixNew", "0.7"},
    {"AlphaMultiplier", "0.999"},
    {"Field1Multiplier", "0.02"},
    {"Field1Bias", "0, 0"},
    {"Field2Multiplier", "0.01"},
    {"Field2Bias", "0, 0"},
    //      {"FieldBias", "-0.5, -0.5"},
  });
    
  // Collage layer from raw pitch/RMS and the raw points FBO
  auto snapshotModPtr = addMod<PixelSnapshotMod>(mods, "Snapshot", {});
  
  auto pathModPtr = addMod<PathMod>(mods, "Collage Path", {
    {"MaxVertices", "7"},
    {"MinVertexProximity", "0.01"},
    {"MaxVertexProximity", "0.07"},
    {"Strategy", "0"} // 0=polypath; 1=bounds; 2=horizontals; 3=convex hull
  });
  
  auto collageModPtr = addMod<CollageMod>(mods, "Collage", {
    {"Strength", "2.0"},
    {"Strategy", "1"}, // 0=tint; 1=add tinted pixels; 2=add pixels
  });
  
  auto collageFadeModPtr = addMod<FadeMod>(mods, "Fade Collage", {
    {"Fade Amount", "0.0005"}
  });
  
  auto outlineFadeModPtr = addMod<FadeMod>(mods, "Fade Collage Outlines", {
    {"Fade Amount", "0.01"}
  });
    
  // DividedArea
  auto dividedAreaModPtr = addMod<DividedAreaMod>(mods, "Divided Area", {
    {"maxConstrainedLines", "1500"},
    {"constrainedWidth", "0.001"},
    {"PathWidth", "0.003"}
  });
    
  // Sandlines
  auto sandLineModPtr = addMod<SandLineMod>(mods, "Sand lines", {
    {"PointRadius", "1.0"},
    {"Density", "0.15"},
    {"AlphaMultiplier", "1.0"},
    {"StdDevAlong", "0.5"},
    {"StdDevPerpendicular", "0.004"}
  });

//    auto fadeModPtr = addMod<FadeMod>(mods, "Fade Sand Lines", {
//      {"Fade Amount", "0.00000005"}
//    });
//    sandLineModPtr->addSink(SandLineMod::SOURCE_FBO, fadeModPtr, FadeMod::SINK_FBO);
//    sandLineModPtr->receive(SandLineMod::SINK_FBO, fboSandlinesPtr);
  
//  { // Cluster particles
//    auto particleSetModPtr = addMod<ParticleSetMod>(mods, "Cluster Particles", {
//      {"maxParticles", "500"},
//      {"maxParticleAge", "300"},
//      {"colourMultiplier", "0.5"}
//    });
//    clusterModPtr->addSink(ClusterMod::SOURCE_VEC2, particleSetModPtr, ParticleSetMod::SINK_POINTS);
//    audioPaletteModPtr->addSink(SomPaletteMod::SOURCE_RANDOM_LIGHT_VEC4, particleSetModPtr, DrawPointsMod::SINK_POINT_COLOR);
//
//    auto fadeModPtr = addMod<FadeMod>(mods, "Fade Particles", {
//      {"Fade Amount", "0.0005"}
//    });
//    particleSetModPtr->addSink(ParticleSetMod::SOURCE_FBO, fadeModPtr, FadeMod::SINK_FBO);
////    auto multiplyModPtr = addMod<MultiplyMod>(mods, "Fade Cluster Particles", {
////      {"Multiply By", "0.995"}
////    });
////    particleSetModPtr->addSink(ParticleSetMod::SOURCE_FBO, multiplyModPtr, MultiplyMod::SINK_FBO);
//
////    particleSetModPtr->receive(ParticleSetMod::SINK_FBO, fluidFboPtr);
//    particleSetModPtr->receive(ParticleSetMod::SINK_FBO, fboClusterParticlesPtr);
//  }
  
  // Could be expressed as: `connectSourceToSinks(sourceModPtr, sourceId, {{destModPtr, destId}, ...})` ?
  // Or with a focus on the sinks: `connectSinkToSources(destModPtr, {{destId, sourceModPtr, sourceId}, ...})` ?
  // The latter probably makes it easier to add/remove Mods.
  audioDataSourceModPtr->connect(AudioDataSourceMod::SOURCE_SPECTRAL_POINTS, audioPaletteModPtr, SomPaletteMod::SINK_VEC3);
  audioDataSourceModPtr->connect(AudioDataSourceMod::SOURCE_POLAR_PITCH_RMS_POINTS, clusterModPtr, ClusterMod::SINK_VEC2);
  audioPaletteModPtr->connect(SomPaletteMod::SOURCE_FIELD, particleFieldModPtr, ParticleFieldMod::SINK_FIELD_1_FBO);
  radiiModPtr->connect(RandomFloatSourceMod::SOURCE_FLOAT, drawPointsModPtr, SoftCircleMod::SINK_POINT_RADIUS);
  audioPaletteModPtr->connect(SomPaletteMod::SOURCE_RANDOM_VEC4, drawPointsModPtr, SoftCircleMod::SINK_POINT_COLOR);
  clusterModPtr->connect(ClusterMod::SOURCE_VEC2, drawPointsModPtr, SoftCircleMod::SINK_POINTS);
  audioPaletteModPtr->connect(SomPaletteMod::SOURCE_RANDOM_DARK_VEC4, rawFluidPointsModPtr, SoftCircleMod::SINK_POINT_COLOR);
  audioDataSourceModPtr->connect(AudioDataSourceMod::SOURCE_PITCH_RMS_POINTS, rawFluidPointsModPtr, SoftCircleMod::SINK_POINTS);
  //    audioDataSourceModPtr->connect(AudioDataSourceMod::SOURCE_POLAR_PITCH_RMS_POINTS, drawPointsModPtr, SoftCircleMod::SINK_POINTS);
  //    audioDataSourceModPtr->addSink(AudioDataSourceMod::SOURCE_SPECTRAL_CREST_SCALAR, drawPointsModPtr, SoftCircleMod::SINK_POINT_RADIUS_VARIANCE);
  //    audioDataSourceModPtr->addSink(AudioDataSourceMod::SOURCE_PITCH_SCALAR, drawPointsModPtr, SoftCircleMod::SINK_POINT_COLOR_MULTIPLIER);
  //    audioDataSourceModPtr->addSink(AudioDataSourceMod::SOURCE_RMS_SCALAR, drawPointsModPtr, SoftCircleMod::SINK_POINT_SOFTNESS);
  audioDataSourceModPtr->connect(AudioDataSourceMod::SOURCE_PITCH_RMS_POINTS, rawFluidRadialImpulseModPtr, FluidRadialImpulseMod::SINK_POINTS);
  //      audioDataSourceModPtr->connect(AudioDataSourceMod::SOURCE_POLAR_PITCH_RMS_POINTS, fluidRadialImpulseModPtr, FluidRadialImpulseMod::SINK_POINTS);
  audioPaletteModPtr->connect(SomPaletteMod::SOURCE_RANDOM_LIGHT_VEC4, smearedPointsModPtr, SoftCircleMod::SINK_POINT_COLOR);
  //    audioDataSourceModPtr->addSink(AudioDataSourceMod::SOURCE_PITCH_RMS_POINTS, smearedPointsModPtr, SoftCircleMod::SINK_POINTS);
  audioDataSourceModPtr->connect(AudioDataSourceMod::SOURCE_POLAR_PITCH_RMS_POINTS, smearedPointsModPtr, SoftCircleMod::SINK_POINTS);
  audioDataSourceModPtr->connect(AudioDataSourceMod::SOURCE_SPECTRAL_CREST_SCALAR, smearedPointsModPtr, SoftCircleMod::SINK_POINT_RADIUS_VARIANCE);
  audioDataSourceModPtr->connect(AudioDataSourceMod::SOURCE_PITCH_SCALAR, smearedPointsModPtr, SoftCircleMod::SINK_POINT_COLOR_MULTIPLIER);
  //    audioDataSourceModPtr->addSink(AudioDataSourceMod::SOURCE_RMS_SCALAR, smearedPointsModPtr, SoftCircleMod::SINK_POINT_SOFTNESS);
  audioDataSourceModPtr->connect(AudioDataSourceMod::SOURCE_POLAR_PITCH_RMS_POINTS, pathModPtr, PathMod::SINK_VEC2);
  audioPaletteModPtr->connect(SomPaletteMod::SOURCE_FIELD, smearModPtr, SmearMod::SINK_FIELD_1_FBO);
  snapshotModPtr->connect(PixelSnapshotMod::SOURCE_SNAPSHOT, collageModPtr, CollageMod::SINK_SNAPSHOT_FBO);
  pathModPtr->connect(PathMod::SOURCE_PATH, collageModPtr, CollageMod::SINK_PATH);
  audioPaletteModPtr->connect(SomPaletteMod::SOURCE_RANDOM_LIGHT_VEC4, collageModPtr, CollageMod::SINK_COLOR);
  clusterModPtr->connect(ClusterMod::SOURCE_VEC2, dividedAreaModPtr, DividedAreaMod::SINK_MAJOR_ANCHORS);
  audioDataSourceModPtr->connect(AudioDataSourceMod::SOURCE_POLAR_PITCH_RMS_POINTS, dividedAreaModPtr, DividedAreaMod::SINK_MINOR_ANCHORS);
  pathModPtr->connect(PathMod::SOURCE_PATH, dividedAreaModPtr, DividedAreaMod::SINK_MINOR_PATH);
  //    audioPaletteModPtr->connect(SomPaletteMod::SOURCE_DARKEST_VEC4, dividedAreaModPtr, DividedAreaMod::SINK_MINOR_LINES_COLOR);
  //    audioPaletteModPtr->connect(SomPaletteMod::SOURCE_DARKEST_VEC4, dividedAreaModPtr, DividedAreaMod::SINK_MAJOR_LINES_COLOR);
  audioPaletteModPtr->connect(SomPaletteMod::SOURCE_RANDOM_LIGHT_VEC4, sandLineModPtr, SandLineMod::SINK_POINT_COLOR);
  clusterModPtr->connect(ClusterMod::SOURCE_VEC2, sandLineModPtr, SandLineMod::SINK_POINTS);
  
  // Could be better expressed as: `connectFboPtr(fboPtr, { {Mod::SINK_FBOPTR, modPtr}, ... })` ?
  particleFieldModPtr->receive(ParticleFieldMod::SINK_FBOPTR, fboMotionParticlesPtr);
  fluidModPtr->receive(FluidMod::SINK_VALUES_FBO, fluidFboPtr);
  fluidModPtr->receive(FluidMod::SINK_VELOCITIES_FBO, fluidVelocitiesFboPtr);
  drawPointsModPtr->receive(SoftCircleMod::SINK_FBOPTR, fluidFboPtr);
  fluidRadialImpulseModPtr->receive(FluidRadialImpulseMod::SINK_FBOPTR, fluidVelocitiesFboPtr);
  rawFluidPointsModPtr->receive(SoftCircleMod::SINK_FBOPTR, fluidFboPtr);
  rawFluidRadialImpulseModPtr->receive(FluidRadialImpulseMod::SINK_FBOPTR, fluidVelocitiesFboPtr);
  drawPointsModPtr->receive(DrawPointsMod::SINK_FBOPTR, rawPointsFboPtr);
  smearModPtr->receive(SmearMod::SINK_FBOPTR, rawPointsFboPtr);
  collageModPtr->receive(CollageMod::SINK_FBOPTR, fboCollagePtr);
  collageFadeModPtr->receive(FadeMod::SINK_FBOPTR, fboCollagePtr);
  collageModPtr->receive(CollageMod::SINK_FBOPTR_2, fboCollageOutlinesPtr);
  outlineFadeModPtr->receive(FadeMod::SINK_FBOPTR, fboCollageOutlinesPtr);
  dividedAreaModPtr->receive(DividedAreaMod::SINK_FBOPTR_2, fboPtrMinorLinesPtr);
  dividedAreaModPtr->receive(DividedAreaMod::SINK_FBOPTR, fboPtrMajorLinesPtr);
  sandLineModPtr->receive(SandLineMod::SINK_FBOPTR, rawPointsFboPtr);
  //    sandLineModPtr->receive(SandLineMod::SINK_FBO, fluidFboPtr);

  return mods;
}

FboConfigPtrs ofApp::createFboConfigs1(glm::vec2 size) {
  // Used by fluid sim but not drawn
  allocateFbo(fluidVelocitiesFboPtr, size / 8.0, GL_RGB16F, GL_REPEAT);
  fluidVelocitiesFboPtr->clearFloat(0.0, 0.0, 0.0, 0.0);
  
  FboConfigPtrs fboConfigPtrs;
  addFboConfigPtr(fboConfigPtrs, "fluid", fluidFboPtr, size / 8.0, GL_RGBA16F, GL_REPEAT, false, OF_BLENDMODE_ALPHA, false, 0);
  addFboConfigPtr(fboConfigPtrs, "raw points", rawPointsFboPtr, size, GL_RGBA16F, GL_REPEAT, false, OF_BLENDMODE_ADD, false, 0);
//  addFboConfigPtr(fboConfigPtrs, "sandlines", fboSandlinesPtr, size, GL_RGBA16F, GL_CLAMP_TO_EDGE, false, OF_BLENDMODE_ADD);
  addFboConfigPtr(fboConfigPtrs, "motion particles", fboMotionParticlesPtr, size/2.0, GL_RGBA, GL_CLAMP_TO_EDGE, false, OF_BLENDMODE_ADD, false, 0);
  addFboConfigPtr(fboConfigPtrs, "minor lines", fboPtrMinorLinesPtr, size, GL_RGBA8, GL_CLAMP_TO_EDGE, true, OF_BLENDMODE_ALPHA, false, 4);
  addFboConfigPtr(fboConfigPtrs, "collage", fboCollagePtr, size, GL_RGBA16F, GL_CLAMP_TO_EDGE, false, OF_BLENDMODE_ADD, true, 0);
  addFboConfigPtr(fboConfigPtrs, "collage outlines", fboCollageOutlinesPtr, size, GL_RGBA16F, GL_CLAMP_TO_EDGE, false, OF_BLENDMODE_ALPHA, false, 0);
//  addFboConfigPtr(fboConfigPtrs, "cluster particles", fboClusterParticlesPtr, size, GL_RGBA16F, GL_CLAMP_TO_EDGE, false, OF_BLENDMODE_ALPHA, false, 0);
  addFboConfigPtr(fboConfigPtrs, "major lines", fboPtrMajorLinesPtr, size, GL_RGBA16F, GL_CLAMP_TO_EDGE, true, OF_BLENDMODE_ALPHA, false, 0); // samples==4 is too much
  return fboConfigPtrs;
}
