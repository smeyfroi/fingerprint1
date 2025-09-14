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
  audioDataSourceModPtr->addSink(AudioDataSourceMod::SOURCE_SPECTRAL_POINTS, audioPaletteModPtr, SomPaletteMod::SINK_VEC3);
  
  // Clusters from raw pitch/RMS points
  auto clusterModPtr = addMod<ClusterMod>(mods, "Clusters", {
    {"maxSourcePoints", "600"},
    {"clusters", "7"}
  });
  audioDataSourceModPtr->addSink(AudioDataSourceMod::SOURCE_POLAR_PITCH_RMS_POINTS, clusterModPtr, ClusterMod::SINK_VEC2);
  
  // Palette ParticleField
  {
    auto particleFieldModPtr = addMod<ParticleFieldMod>(mods, "Palette Particle Field", {
      {"velocityDamping", "0.992"},
      {"forceMultiplier", "0.01"},
      {"maxVelocity", "0.001"},
      {"particleSize", "6.0"}
    }, -0.5);
    audioPaletteModPtr->addSink(SomPaletteMod::SOURCE_FIELD, particleFieldModPtr, ParticleFieldMod::SINK_FIELD_FBO);
    particleFieldModPtr->receive(ParticleFieldMod::SINK_FBO, fboMotionParticlesPtr);
  }
  
//  // Fluid simulation
//  auto fluidModPtr = addMod<FluidMod>(mods, "Fluid", {
//    {"dt", "0.005"},
//    {"value:dissipation", "0.9999"},
//    {"velocity:dissipation", "0.999"},
//    {"vorticity", "20.0"}
//  });
//  fluidModPtr->receive(FluidMod::SINK_VALUES_FBO, fluidFboPtr);
//  fluidModPtr->receive(FluidMod::SINK_VELOCITIES_FBO, fluidVelocitiesFboPtr);

//  // Clusters into fluid values
//  {
//    auto radiiModPtr = addMod<RandomFloatSourceMod>(mods, "Fluid Points Radii", {
//      {"CreatedPerUpdate", "0.05"},
//      {"Min", "0.005"},
//      {"Max", "0.05"}
//    }, std::pair<float, float>{0.0, 0.1}, std::pair<float, float>{0.0, 0.1});
//    auto drawPointsModPtr = addMod<SoftCircleMod>(mods, "Draw Fluid Points", {
//      {"ColorMultiplier", "0.1"}
//    });
//    radiiModPtr->addSink(RandomFloatSourceMod::SOURCE_FLOAT, drawPointsModPtr, DrawPointsMod::SINK_POINT_RADIUS);
//    audioPaletteModPtr->addSink(SomPaletteMod::SOURCE_RANDOM_VEC4, drawPointsModPtr, DrawPointsMod::SINK_POINT_COLOR);
//    clusterModPtr->addSink(ClusterMod::SOURCE_VEC2, drawPointsModPtr, DrawPointsMod::SINK_POINTS);
//    drawPointsModPtr->receive(DrawPointsMod::SINK_FBO, fluidFboPtr);
//  }

//  { // Radial impulses from clusters
//    auto fluidRadialImpulseModPtr = addMod<FluidRadialImpulseMod>(mods, "Cluster Impulses", {
//      {"ImpulseRadius", "0.15"},
//      {"ImpulseStrength", "0.15"}
//    });
//    clusterModPtr->addSink(ClusterMod::SOURCE_VEC2, fluidRadialImpulseModPtr, FluidRadialImpulseMod::SINK_POINTS);
//    fluidRadialImpulseModPtr->receive(FluidRadialImpulseMod::SINK_FBO, fluidVelocitiesFboPtr);
//  }

//  { // Raw data points into fluid
//    auto drawPointsModPtr = addMod<SoftCircleMod>(mods, "Fluid Raw Points", {
//      {"Radius", "0.001"},
////      {"ColorMultiplier", "0.7"}
//    });
//    audioPaletteModPtr->addSink(SomPaletteMod::SOURCE_RANDOM_DARK_VEC4, drawPointsModPtr, DrawPointsMod::SINK_POINT_COLOR);
////    audioDataSourceModPtr->addSink(AudioDataSourceMod::SOURCE_PITCH_RMS_POINTS, drawPointsModPtr, DrawPointsMod::SINK_POINTS);
//    audioDataSourceModPtr->addSink(AudioDataSourceMod::SOURCE_POLAR_PITCH_RMS_POINTS, drawPointsModPtr, DrawPointsMod::SINK_POINTS);
//    audioDataSourceModPtr->addSink(AudioDataSourceMod::SOURCE_SPECTRAL_CREST_SCALAR, drawPointsModPtr, SoftCircleMod::SINK_POINT_RADIUS_VARIANCE);
//    audioDataSourceModPtr->addSink(AudioDataSourceMod::SOURCE_PITCH_SCALAR, drawPointsModPtr, SoftCircleMod::SINK_POINT_COLOR_MULTIPLIER);
//    audioDataSourceModPtr->addSink(AudioDataSourceMod::SOURCE_RMS_SCALAR, drawPointsModPtr, SoftCircleMod::SINK_POINT_SOFTNESS);
//    drawPointsModPtr->receive(DrawPointsMod::SINK_FBO, fluidFboPtr);
//  }
  
  { // Smeared raw data points
    auto drawPointsModPtr = addMod<SoftCircleMod>(mods, "Raw Points", {
      {"Radius", "0.001"}, // min
      {"RadiusVarianceScale", "0.01"}, // scale the variance to some to the min
    });
    audioPaletteModPtr->addSink(SomPaletteMod::SOURCE_RANDOM_LIGHT_VEC4, drawPointsModPtr, SoftCircleMod::SINK_POINT_COLOR);
//    audioDataSourceModPtr->addSink(AudioDataSourceMod::SOURCE_PITCH_RMS_POINTS, drawPointsModPtr, SoftCircleMod::SINK_POINTS);
    audioDataSourceModPtr->addSink(AudioDataSourceMod::SOURCE_POLAR_PITCH_RMS_POINTS, drawPointsModPtr, SoftCircleMod::SINK_POINTS);
    audioDataSourceModPtr->addSink(AudioDataSourceMod::SOURCE_SPECTRAL_CREST_SCALAR, drawPointsModPtr, SoftCircleMod::SINK_POINT_RADIUS_VARIANCE);
    audioDataSourceModPtr->addSink(AudioDataSourceMod::SOURCE_PITCH_SCALAR, drawPointsModPtr, SoftCircleMod::SINK_POINT_COLOR_MULTIPLIER);
    audioDataSourceModPtr->addSink(AudioDataSourceMod::SOURCE_RMS_SCALAR, drawPointsModPtr, SoftCircleMod::SINK_POINT_SOFTNESS);

    auto smearModPtr = addMod<SmearMod>(mods, "Smear Raw Points", {
      {"Translation", "0.0, 0.0005"},
      {"MixNew", "0.8"},
      {"AlphaMultiplier", "0.999"},
    });
    drawPointsModPtr->addSink(DrawPointsMod::SOURCE_FBO, smearModPtr, SmearMod::SINK_FBO);

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
    audioDataSourceModPtr->addSink(AudioDataSourceMod::SOURCE_POLAR_PITCH_RMS_POINTS, pathModPtr, PathMod::SINK_VEC2);

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
    clusterModPtr->addSink(ClusterMod::SOURCE_VEC2, dividedAreaModPtr, DividedAreaMod::SINK_MAJOR_ANCHORS);
//    audioDataSourceModPtr->addSink(AudioDataSourceMod::SOURCE_POLAR_PITCH_RMS_POINTS, dividedAreaModPtr, DividedAreaMod::SINK_MINOR_ANCHORS);
    pathModPtr->addSink(PathMod::SOURCE_PATH, dividedAreaModPtr, DividedAreaMod::SINK_MINOR_PATH);

    dividedAreaModPtr->receive(DividedAreaMod::SINK_FBO_2, fboPtrMinorLinesPtr);
    dividedAreaModPtr->receive(DividedAreaMod::SINK_FBO, fboPtrMajorLinesPtr);
  }
  
//  { // Sandlines
//    auto sandLineModPtr = addMod<SandLineMod>(mods, "Sand lines", {
//      {"PointRadius", "1.0"},
//      {"Density", "0.15"},
//      {"AlphaMultiplier", "1.0"},
//      {"StdDevAlong", "0.5"},
//      {"StdDevPerpendicular", "0.004"}
//    });
//    audioPaletteModPtr->addSink(SomPaletteMod::SOURCE_RANDOM_LIGHT_VEC4, sandLineModPtr, SandLineMod::SINK_POINT_COLOR);
//    clusterModPtr->addSink(ClusterMod::SOURCE_VEC2, sandLineModPtr, SandLineMod::SINK_POINTS);
//    sandLineModPtr->receive(SandLineMod::SINK_FBO, rawPointsFboPtr);
////    sandLineModPtr->receive(SandLineMod::SINK_FBO, fluidFboPtr);
//
////    auto fadeModPtr = addMod<FadeMod>(mods, "Fade Sand Lines", {
////      {"Fade Amount", "0.00000005"}
////    });
////    sandLineModPtr->addSink(SandLineMod::SOURCE_FBO, fadeModPtr, FadeMod::SINK_FBO);
////    sandLineModPtr->receive(SandLineMod::SINK_FBO, fboSandlinesPtr);
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
  
  return mods;
}

FboConfigPtrs ofApp::createFboConfigs1(glm::vec2 size) {
  // Used by fluid sim but not drawn
  allocateFbo(fluidVelocitiesFboPtr, size / 8.0, GL_RGB16F, GL_REPEAT);
  fluidVelocitiesFboPtr->getSource().begin();
  fluidVelocitiesFboPtr->getSource().clearColorBuffer({ 0.0, 0.0, 0.0 });
  fluidVelocitiesFboPtr->getSource().end();
  
  FboConfigPtrs fboConfigPtrs;
  const ofFloatColor backgroundColor { 0.0, 0.0, 0.0, 0.0 };
  addFboConfigPtr(fboConfigPtrs, "fluid", fluidFboPtr, size / 8.0, GL_RGBA16F, GL_REPEAT, backgroundColor, false, OF_BLENDMODE_ALPHA, false);
  addFboConfigPtr(fboConfigPtrs, "raw points", rawPointsFboPtr, size, GL_RGBA16F, GL_REPEAT, backgroundColor, false, OF_BLENDMODE_ADD, false);
//  addFboConfigPtr(fboConfigPtrs, "sandlines", fboSandlinesPtr, size, GL_RGBA16F, GL_CLAMP_TO_EDGE, backgroundColor, false, OF_BLENDMODE_ADD);
  addFboConfigPtr(fboConfigPtrs, "motion particles", fboMotionParticlesPtr, size/2.0, GL_RGBA, GL_CLAMP_TO_EDGE, backgroundColor, false, OF_BLENDMODE_ADD, false);
  addFboConfigPtr(fboConfigPtrs, "minor lines", fboPtrMinorLinesPtr, size, GL_RGBA8, GL_CLAMP_TO_EDGE, backgroundColor, true, OF_BLENDMODE_ALPHA, false);
  addFboConfigPtr(fboConfigPtrs, "collage", fboCollagePtr, size, GL_RGBA16F, GL_CLAMP_TO_EDGE, backgroundColor, false, OF_BLENDMODE_ADD, true);
  addFboConfigPtr(fboConfigPtrs, "cluster particles", fboClusterParticlesPtr, size, GL_RGBA16F, GL_CLAMP_TO_EDGE, backgroundColor, false, OF_BLENDMODE_ALPHA, false);
  addFboConfigPtr(fboConfigPtrs, "major lines", fboPtrMajorLinesPtr, size, GL_RGBA16F, GL_CLAMP_TO_EDGE, backgroundColor, true, OF_BLENDMODE_ALPHA, false);
  return fboConfigPtrs;
}
