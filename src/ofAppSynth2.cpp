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
    {"MaxRms", "0.11"},
    {"MinComplexSpectralDifference", "200.0"},
    {"MaxComplexSpectralDifference", "1000.0"},
    {"MinSpectralCrest", "20.0"},
    {"MaxSpectralCrest", "350.0"},
    {"MinZeroCrossingRate", "5.0"},
    {"MaxZeroCrossingRate", "15.0"}
  }, audioDataProcessorPtr);
  
  // Palette from raw spectral points
  auto audioPaletteModPtr = addMod<SomPaletteMod>(mods, "Palette Creator", {
    {"Iterations", "4000"}
  });
  audioDataSourceModPtr->connect(AudioDataSourceMod::SOURCE_SPECTRAL_POINTS, audioPaletteModPtr, SomPaletteMod::SINK_VEC3);

  // Clusters from raw pitch/RMS points
  auto clusterModPtr = addMod<ClusterMod>(mods, "Clusters", {
    {"maxSourcePoints", "600"},
    {"clusters", "7"}
  });
  audioDataSourceModPtr->connect(AudioDataSourceMod::SOURCE_POLAR_PITCH_RMS_POINTS, clusterModPtr, ClusterMod::SINK_VEC2);

  { // Smeared raw data points
    auto drawPointsModPtr = addMod<SoftCircleMod>(mods, "Raw Points", {
      {"Radius", "0.005"}, // min
      {"RadiusVarianceScale", "0.01"}, // scale the variance to some to the min
      {"Softness", "1.0"},
    });
    audioPaletteModPtr->connect(SomPaletteMod::SOURCE_RANDOM_LIGHT_VEC4, drawPointsModPtr, SoftCircleMod::SINK_POINT_COLOR);
//    audioDataSourceModPtr->addSink(AudioDataSourceMod::SOURCE_PITCH_RMS_POINTS, drawPointsModPtr, SoftCircleMod::SINK_POINTS);
    audioDataSourceModPtr->connect(AudioDataSourceMod::SOURCE_POLAR_PITCH_RMS_POINTS, drawPointsModPtr, SoftCircleMod::SINK_POINTS);
    audioDataSourceModPtr->connect(AudioDataSourceMod::SOURCE_SPECTRAL_CREST_SCALAR, drawPointsModPtr, SoftCircleMod::SINK_POINT_RADIUS_VARIANCE);
    audioDataSourceModPtr->connect(AudioDataSourceMod::SOURCE_PITCH_SCALAR, drawPointsModPtr, SoftCircleMod::SINK_POINT_COLOR_MULTIPLIER);

    auto smearModPtr = addMod<SmearMod>(mods, "Smear Raw Points", {
      {"Translation", "0.0, 0.0005"},
      {"MixNew", "0.8"},
      {"AlphaMultiplier", "0.999"},
    });
    drawPointsModPtr->connect(DrawPointsMod::SOURCE_FBO, smearModPtr, SmearMod::SINK_FBO);

    drawPointsModPtr->receive(DrawPointsMod::SINK_FBO, rawPointsFboPtr);
  }

  { // Collage layer from raw pitch/RMS and the raw points FBO
//    auto pixelSnapshotModPtr = addMod<PixelSnapshotMod>(mods, "Snapshot", {});
//    pixelSnapshotModPtr->receive(PixelSnapshotMod::SINK_FBO, fluidFboPtr);
////    pixelSnapshotModPtr->receive(PixelSnapshotMod::SINK_FBO, rawPointsFboPtr);

    auto pathModPtr = addMod<PathMod>(mods, "Collage Path", {
      {"MaxVertices", "7"},
      {"VertexProximity", "0.05"},
      {"Strategy", "1"} // 0=polypath; 1=bounds; 2=horizontals; 3=convex hull
    });
    audioDataSourceModPtr->connect(AudioDataSourceMod::SOURCE_POLAR_PITCH_RMS_POINTS, pathModPtr, PathMod::SINK_VEC2);

//    // DON'T DO THIS IF COLLAGE IS DRAWIN INTO THE FLUID FBO
////    auto fluidCollageModPtr = addMod<CollageMod>(mods, "Fluid Collage", {
////      {"Strength", "1.0"},
////      {"Strategy", "1"}, // 0=tint; 1=add tinted pixels
////    });
////    pixelSnapshotModPtr->addSink(PixelSnapshotMod::SOURCE_PIXELS, fluidCollageModPtr, CollageMod::SINK_PIXELS);
////    pathModPtr->addSink(PathMod::SOURCE_PATH, fluidCollageModPtr, CollageMod::SINK_PATH);
////    audioPaletteModPtr->addSink(SomPaletteMod::SOURCE_RANDOM_VEC4, fluidCollageModPtr, CollageMod::SINK_COLOR);
////    fluidCollageModPtr->receive(CollageMod::SINK_FBO, fluidFboPtr);
//
//    auto collageModPtr = addMod<CollageMod>(mods, "Collage", {
//      {"Strength", "1.0"},
//      {"Strategy", "1"}, // 0=tint; 1=add tinted pixels
//    });
//    pixelSnapshotModPtr->addSink(PixelSnapshotMod::SOURCE_PIXELS, collageModPtr, CollageMod::SINK_PIXELS);
//    pathModPtr->addSink(PathMod::SOURCE_PATH, collageModPtr, CollageMod::SINK_PATH);
//    audioPaletteModPtr->addSink(SomPaletteMod::SOURCE_RANDOM_LIGHT_VEC4, collageModPtr, CollageMod::SINK_COLOR);
//
//    auto fadeModPtr = addMod<FadeMod>(mods, "Fade Collage", {
//      {"Fade Amount", "0.001"}
//    });
//    collageModPtr->addSink(CollageMod::SOURCE_FBO, fadeModPtr, FadeMod::SINK_FBO);
//
//    collageModPtr->receive(CollageMod::SINK_FBO, fboCollagePtr);

    // DividedArea
    auto dividedAreaModPtr = addMod<DividedAreaMod>(mods, "Divided Area", {});
    clusterModPtr->connect(ClusterMod::SOURCE_VEC2, dividedAreaModPtr, DividedAreaMod::SINK_MAJOR_ANCHORS);
//    audioDataSourceModPtr->addSink(AudioDataSourceMod::SOURCE_POLAR_PITCH_RMS_POINTS, dividedAreaModPtr, DividedAreaMod::SINK_MINOR_ANCHORS);
    pathModPtr->connect(PathMod::SOURCE_PATH, dividedAreaModPtr, DividedAreaMod::SINK_MINOR_PATH);

    dividedAreaModPtr->receive(DividedAreaMod::SINK_FBO_2, fboPtrMinorLinesPtr);
    dividedAreaModPtr->receive(DividedAreaMod::SINK_FBO, fboPtrMajorLinesPtr);
  }

  { // Sandlines
    auto sandLineModPtr = addMod<SandLineMod>(mods, "Sand lines", {
      {"PointRadius", "1.0"},
      {"Density", "0.15"},
      {"AlphaMultiplier", "1.0"},
      {"StdDevAlong", "0.5"},
      {"StdDevPerpendicular", "0.004"}
    });
    audioPaletteModPtr->connect(SomPaletteMod::SOURCE_RANDOM_LIGHT_VEC4, sandLineModPtr, SandLineMod::SINK_POINT_COLOR);
    clusterModPtr->connect(ClusterMod::SOURCE_VEC2, sandLineModPtr, SandLineMod::SINK_POINTS);
    sandLineModPtr->receive(SandLineMod::SINK_FBO, rawPointsFboPtr);
  }
  
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
  fluidModPtr->receive(FluidMod::SINK_VALUES_FBO, fluidFboPtr);
  fluidModPtr->receive(FluidMod::SINK_VELOCITIES_FBO, fluidVelocitiesFboPtr);

  // Clusters into fluid values
  {
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
    radiiModPtr->connect(RandomFloatSourceMod::SOURCE_FLOAT, drawPointsModPtr, SoftCircleMod::SINK_POINT_RADIUS);
    audioPaletteModPtr->connect(SomPaletteMod::SOURCE_RANDOM_VEC4, drawPointsModPtr, SoftCircleMod::SINK_POINT_COLOR);
    clusterModPtr->connect(ClusterMod::SOURCE_VEC2, drawPointsModPtr, SoftCircleMod::SINK_POINTS);
    drawPointsModPtr->receive(SoftCircleMod::SINK_FBO, fluidFboPtr);
  }

  { // Radial fluid impulses from clusters
    auto fluidRadialImpulseModPtr = addMod<FluidRadialImpulseMod>(mods, "Cluster Impulses", {
      {"ImpulseRadius", "0.05"},
      {"ImpulseStrength", "0.02"}
    });
    clusterModPtr->connect(ClusterMod::SOURCE_VEC2, fluidRadialImpulseModPtr, FluidRadialImpulseMod::SINK_POINTS);
    fluidRadialImpulseModPtr->receive(FluidRadialImpulseMod::SINK_FBO, fluidVelocitiesFboPtr);
  }

  { // Raw data points into fluid
    auto drawPointsModPtr = addMod<SoftCircleMod>(mods, "Fluid Raw Points", {
      {"Radius", "0.01"},
      {"ColorMultiplier", "0.4"},
      {"AlphaMultiplier", "0.85"},
      {"Softness", "0.1"}
    });
    audioPaletteModPtr->connect(SomPaletteMod::SOURCE_RANDOM_DARK_VEC4, drawPointsModPtr, SoftCircleMod::SINK_POINT_COLOR);
//    audioDataSourceModPtr->addSink(AudioDataSourceMod::SOURCE_PITCH_RMS_POINTS, drawPointsModPtr, DrawPointsMod::SINK_POINTS);
    audioDataSourceModPtr->connect(AudioDataSourceMod::SOURCE_POLAR_PITCH_RMS_POINTS, drawPointsModPtr, SoftCircleMod::SINK_POINTS);
//    audioDataSourceModPtr->addSink(AudioDataSourceMod::SOURCE_SPECTRAL_CREST_SCALAR, drawPointsModPtr, SoftCircleMod::SINK_POINT_RADIUS_VARIANCE);
//    audioDataSourceModPtr->addSink(AudioDataSourceMod::SOURCE_PITCH_SCALAR, drawPointsModPtr, SoftCircleMod::SINK_POINT_COLOR_MULTIPLIER);
//    audioDataSourceModPtr->addSink(AudioDataSourceMod::SOURCE_RMS_SCALAR, drawPointsModPtr, SoftCircleMod::SINK_POINT_SOFTNESS);
    drawPointsModPtr->receive(SoftCircleMod::SINK_FBO, fluidFboPtr);
    
    { // Radial fluid impulses from raw points
      auto fluidRadialImpulseModPtr = addMod<FluidRadialImpulseMod>(mods, "Raw Point Impulses", {
        {"ImpulseRadius", "0.02"},
        {"ImpulseStrength", "0.06"}
      });
      audioDataSourceModPtr->connect(AudioDataSourceMod::SOURCE_POLAR_PITCH_RMS_POINTS, fluidRadialImpulseModPtr, FluidRadialImpulseMod::SINK_POINTS);
      fluidRadialImpulseModPtr->receive(FluidRadialImpulseMod::SINK_FBO, fluidVelocitiesFboPtr);
    }
  }
  
  auto videoFlowModPtr = addMod<VideoFlowSourceMod>(mods, "Video flow", {
    { "offset", "2.0" },
    { "threshold", "0.4" },
    { "force", "5.0" },
    { "power", "0.8" }
  }, VIDEO_DEVICE_ID, glm::vec2 { 1280, 720 }, RECORD_VIDEO, saveFilePath("video-recordings"));
//  auto videoFlowModPtr = addMod<VideoFlowSourceMod>(mods, "Video flow", {}, rootSourceMaterialPath/"trombone-trimmed.mov", true);
  
  // Video ParticleField
  {
    auto particleFieldModPtr = addMod<ParticleFieldMod>(mods, "Video Motion Particle Field", {
      {"velocityDamping", "0.995"},
      {"forceMultiplier", "0.5"},
      {"maxVelocity", "0.0005"},
      {"particleSize", "3.0"}
    }, 0.0);
    videoFlowModPtr->connect(VideoFlowSourceMod::SOURCE_FLOW_FBO, particleFieldModPtr, ParticleFieldMod::SINK_FIELD_FBO);
    particleFieldModPtr->receive(ParticleFieldMod::SINK_FBO, fboMotionParticlesPtr);
  }
  
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

  return mods;
}

FboConfigPtrs ofApp::createFboConfigs2(glm::vec2 size) {
  // Used by fluid sim but not drawn
  allocateFbo(fluidVelocitiesFboPtr, size / 8.0, GL_RGB16F, GL_REPEAT);
  fluidVelocitiesFboPtr->clearFloat(0.0, 0.0, 0.0, 0.0);
  
  FboConfigPtrs fboConfigPtrs;
  const ofFloatColor backgroundColor { 0.0, 0.0, 0.0, 0.0 };
  addFboConfigPtr(fboConfigPtrs, "fluid", fluidFboPtr, size / 8.0, GL_RGBA16F, GL_REPEAT, backgroundColor, false, OF_BLENDMODE_ALPHA, false, 0);
  addFboConfigPtr(fboConfigPtrs, "major lines", fboPtrMajorLinesPtr, size, GL_RGBA32F, GL_CLAMP_TO_EDGE, backgroundColor, true, OF_BLENDMODE_ALPHA, false, 0);
  addFboConfigPtr(fboConfigPtrs, "sandlines", fboSandlinesPtr, size, GL_RGBA32F, GL_CLAMP_TO_EDGE, backgroundColor, false, OF_BLENDMODE_ADD, false, 0);
  addFboConfigPtr(fboConfigPtrs, "collage", fboCollagePtr, size, GL_RGBA32F, GL_CLAMP_TO_EDGE, backgroundColor, false, OF_BLENDMODE_ALPHA, false, 0);
  addFboConfigPtr(fboConfigPtrs, "raw points", rawPointsFboPtr, size, FLOAT_A_MODE, GL_REPEAT, backgroundColor, false, OF_BLENDMODE_ALPHA, false, 0);
  addFboConfigPtr(fboConfigPtrs, "cluster particles", fboClusterParticlesPtr, size, GL_RGBA32F, GL_CLAMP_TO_EDGE, backgroundColor, false, OF_BLENDMODE_ALPHA, false, 0);
  addFboConfigPtr(fboConfigPtrs, "motion particles", fboMotionParticlesPtr, size/4.0, GL_RGBA, GL_CLAMP_TO_EDGE, backgroundColor, true, OF_BLENDMODE_ADD, false, 0);
  addFboConfigPtr(fboConfigPtrs, "minor lines", fboPtrMinorLinesPtr, size, GL_RGBA8, GL_CLAMP_TO_EDGE, backgroundColor, true, OF_BLENDMODE_ALPHA, false, 0);
  return fboConfigPtrs;
}
