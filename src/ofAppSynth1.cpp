//
//  ofAppSynth1.cpp
//  fingerprint1
//
//  Created by Steve Meyfroidt on 06/06/2025.
//

#include "ofApp.h"

using namespace ofxMarkSynth;

std::shared_ptr<Synth> createSynth1(glm::vec2 size) {
  auto synthPtr = std::make_shared<Synth>("Synth1", ModConfig {}, START_PAUSED, size);

  // Audio source
  std::shared_ptr<AudioDataSourceMod> audioDataSourceModPtr = std::static_pointer_cast<AudioDataSourceMod>(synthPtr->addMod<AudioDataSourceMod>("Audio Source", {
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
  }, MIC_DEVICE_NAME, RECORD_AUDIO, RECORDING_PATH));
  //  audioDataSourceModPtr->registerAudioCallback([this](const float* audioBuffer, size_t bufferSize, int numChannels, int sampleRate) {
  //    ofLogNotice() << audioBuffer[0];
  //  });
  
  // Palette from raw spectral points
  auto audioPaletteModPtr = synthPtr->addMod<SomPaletteMod>("Palette Creator", {
    {"Iterations", "4000"}
  });
  
  // Clusters from raw pitch/RMS points
  auto clusterModPtr = synthPtr->addMod<ClusterMod>("Clusters", {
    {"maxSourcePoints", "600"},
    {"clusters", "7"}
  });
  
  // Palette ParticleField
  auto particleFieldModPtr = synthPtr->addMod<ParticleFieldMod>("Particle Field", {
    {"velocityDamping", "0.994"},
    {"forceMultiplier", "0.2"},
    {"maxVelocity", "0.0001"},
    {"particleSize", "1.0"}
  }, 0.0, 0.0, 500'000);
  
  // Fluid simulation
  auto fluidModPtr = synthPtr->addMod<FluidMod>("Fluid", {
    {"dt", "0.007"},
    {"value:dissipation", "0.999"},
    {"velocity:dissipation", "0.998"},
    {"vorticity", "50.0"},
    {"value:iterations", "0.0"},
    {"velocity:iterations", "0.0"},
    {"pressure:iterations", "25.0"}
  });
  
  // Clusters into fluid values
  auto radiiModPtr = synthPtr->addMod<RandomFloatSourceMod>("Fluid Cluster Radius", {
    {"CreatedPerUpdate", "0.05"},
    {"Min", "0.005"},
    {"Max", "0.02"}
  }, std::pair<float, float>{0.0, 0.1}, std::pair<float, float>{0.0, 0.1});
  
  auto drawPointsModPtr = synthPtr->addMod<SoftCircleMod>("Fluid Clusters", {
    {"ColorMultiplier", "0.5"},
    {"AlphaMultiplier", "0.3"},
    {"Softness", "0.3"}
  });
  
  // Radial fluid impulses from clusters
  auto fluidRadialImpulseModPtr = synthPtr->addMod<FluidRadialImpulseMod>("Cluster Impulses", {
    {"ImpulseRadius", "0.05"},
    {"ImpulseStrength", "0.02"}
  });
  
  // Raw data points into fluid
  auto rawFluidPointsModPtr = synthPtr->addMod<SoftCircleMod>("Fluid Raw Points", {
    {"Radius", "0.01"},
    {"ColorMultiplier", "0.4"},
    {"AlphaMultiplier", "0.6"},
    {"Softness", "0.5"}
  });
  
  // Radial fluid impulses from raw points
  auto rawFluidRadialImpulseModPtr = synthPtr->addMod<FluidRadialImpulseMod>("Raw Point Impulses", {
    {"ImpulseRadius", "0.025"},
    {"ImpulseStrength", "0.08"}
  });
  
  // Smeared raw data points
  auto smearedPointsModPtr = synthPtr->addMod<SoftCircleMod>("Raw Points", {
    {"Radius", "0.005"}, // min
    {"RadiusVarianceScale", "0.01"}, // scale the variance to some to the min
    {"Softness", "0.5"},
    {"AlphaMultiplier", "0.9"},
    {"ColorMultiplier", "0.8"}
  });
  
  auto smearModPtr = synthPtr->addMod<SmearMod>("Smear Raw Points", {
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
  auto snapshotModPtr = synthPtr->addMod<PixelSnapshotMod>("Snapshot", {});
  
  auto pathModPtr = synthPtr->addMod<PathMod>("Collage Path", {
    {"MaxVertices", "7"},
    {"MinVertexProximity", "0.01"},
    {"MaxVertexProximity", "0.07"},
    {"Strategy", "0"} // 0=polypath; 1=bounds; 2=horizontals; 3=convex hull
  });
  
  auto collageModPtr = synthPtr->addMod<CollageMod>("Collage", {
    {"Strength", "2.0"},
    {"Strategy", "1"}, // 0=tint; 1=add tinted pixels; 2=add pixels
  });
  
  auto collageFadeModPtr = synthPtr->addMod<FadeMod>("Fade Collage", {
    {"Fade Amount", "0.0005"}
  });
  
  auto outlineFadeModPtr = synthPtr->addMod<FadeMod>("Fade Collage Outlines", {
    {"Fade Amount", "0.01"}
  });
  
  // DividedArea
  auto dividedAreaModPtr = synthPtr->addMod<DividedAreaMod>("Divided Area", {
    {"maxConstrainedLines", "1500"},
    {"constrainedWidth", "0.001"},
    {"PathWidth", "0.003"}
  });
  
  // Sandlines
  auto sandLineModPtr = synthPtr->addMod<SandLineMod>("Sand lines", {
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
  
  connectSourceToSinks(audioDataSourceModPtr, {
    { AudioDataSourceMod::SOURCE_SPECTRAL_CREST_SCALAR, {
      { smearedPointsModPtr, SoftCircleMod::SINK_POINT_RADIUS_VARIANCE }}
    },
    { AudioDataSourceMod::SOURCE_SPECTRAL_POINTS, {
      { audioPaletteModPtr, SomPaletteMod::SINK_VEC3 }}
    },
    { AudioDataSourceMod::SOURCE_PITCH_SCALAR, {
      { smearedPointsModPtr, SoftCircleMod::SINK_POINT_COLOR_MULTIPLIER }}
    },
    { AudioDataSourceMod::SOURCE_POLAR_PITCH_RMS_POINTS, {
      { clusterModPtr, ClusterMod::SINK_VEC2 },
      { smearedPointsModPtr, SoftCircleMod::SINK_POINTS },
      { pathModPtr, PathMod::SINK_VEC2 },
      { dividedAreaModPtr, DividedAreaMod::SINK_MINOR_ANCHORS }}
    },
    { AudioDataSourceMod::SOURCE_PITCH_RMS_POINTS, {
      { rawFluidPointsModPtr, SoftCircleMod::SINK_POINTS },
      { rawFluidRadialImpulseModPtr, FluidRadialImpulseMod::SINK_POINTS }}
    },
    { AudioDataSourceMod::SOURCE_ONSET1, {
      { synthPtr, Synth::SINK_AUDIO_ONSET }}
    },
    { AudioDataSourceMod::SOURCE_TIMBRE_CHANGE, {
      { synthPtr, Synth::SINK_AUDIO_TIMBRE_CHANGE }}
    }
  });
  connectSourceToSinks(audioPaletteModPtr, {
    { SomPaletteMod::SOURCE_FIELD, {
      { particleFieldModPtr, ParticleFieldMod::SINK_FIELD_1_FBO },
      { smearModPtr, SmearMod::SINK_FIELD_1_FBO }}
    },
    { SomPaletteMod::SOURCE_RANDOM_VEC4, {
      { drawPointsModPtr, SoftCircleMod::SINK_POINT_COLOR }}
    },
    { SomPaletteMod::SOURCE_RANDOM_DARK_VEC4, {
      { rawFluidPointsModPtr, SoftCircleMod::SINK_POINT_COLOR }}
    },
    { SomPaletteMod::SOURCE_DARKEST_VEC4, {
      { synthPtr, Synth::SINK_BACKGROUND_COLOR }}
    },
    { SomPaletteMod::SOURCE_RANDOM_LIGHT_VEC4, {
      { smearedPointsModPtr, SoftCircleMod::SINK_POINT_COLOR },
      { collageModPtr, CollageMod::SINK_COLOR },
      { sandLineModPtr, SandLineMod::SINK_POINT_COLOR }}
    }
  });
  connectSourceToSinks(radiiModPtr, {
    { RandomFloatSourceMod::SOURCE_FLOAT, {
      { drawPointsModPtr, SoftCircleMod::SINK_POINT_RADIUS }}
    }
  });
  connectSourceToSinks(clusterModPtr, {
    { ClusterMod::SOURCE_VEC2, {
      { drawPointsModPtr, SoftCircleMod::SINK_POINTS },
      { fluidRadialImpulseModPtr, FluidRadialImpulseMod::SINK_POINTS },
      { dividedAreaModPtr, DividedAreaMod::SINK_MAJOR_ANCHORS },
      { sandLineModPtr, SandLineMod::SINK_POINTS }}
    }
  });
  connectSourceToSinks(snapshotModPtr, {
    { PixelSnapshotMod::SOURCE_SNAPSHOT, {
      { collageModPtr, CollageMod::SINK_SNAPSHOT_FBO }}
    }
  });
  connectSourceToSinks(pathModPtr, {
    { PathMod::SOURCE_PATH, {
      { collageModPtr, CollageMod::SINK_PATH },
      { dividedAreaModPtr, DividedAreaMod::SINK_MINOR_PATH }}
    }
  });
  connectSourceToSinks(synthPtr, {
    { Synth::SOURCE_COMPOSITE_FBO, {
      { snapshotModPtr, PixelSnapshotMod::SINK_SNAPSHOT_SOURCE },
      { dividedAreaModPtr, DividedAreaMod::SINK_BACKGROUND_SOURCE }}
    }
  });

  // Make some drawing layers
  auto fluidDrawingLayerPtr = synthPtr->addDrawingLayer("1fluid",
                                            synthPtr->getSize() / 8.0, GL_RGBA16F, GL_REPEAT,
                                            false, OF_BLENDMODE_ALPHA, false, 0);
  auto fluidVelocitiesDrawingLayerPtr = synthPtr->addDrawingLayer("fluidVelocities",
                                                      synthPtr->getSize() / 8.0, GL_RGB16F, GL_REPEAT,
                                                      false, OF_BLENDMODE_DISABLED, false, 0, false); // not drawn
  auto rawPointsDrawingLayerPtr = synthPtr->addDrawingLayer("2raw points",
                                                synthPtr->getSize(), GL_RGBA16F, GL_REPEAT,
                                                false, OF_BLENDMODE_ADD, false, 0);
  //  addFboConfigPtr(fboConfigPtrs, "sandlines", fboSandlinesPtr, size, GL_RGBA16F, GL_CLAMP_TO_EDGE, false, OF_BLENDMODE_ADD);
  auto motionParticlesDrawingLayerPtr = synthPtr->addDrawingLayer("3motion particles",
                                                      synthPtr->getSize()/2.0, GL_RGBA, GL_CLAMP_TO_EDGE,
                                                      false, OF_BLENDMODE_ADD, false, 0);
  auto minorLinesDrawingLayerPtr = synthPtr->addDrawingLayer("4minor lines",
                                                    synthPtr->getSize(), GL_RGBA8, GL_CLAMP_TO_EDGE,
                                                    true, OF_BLENDMODE_ALPHA, false, 2);
  auto collageDrawingLayerPtr = synthPtr->addDrawingLayer("5collage",
                                              synthPtr->getSize(), GL_RGBA16F, GL_CLAMP_TO_EDGE,
                                              false, OF_BLENDMODE_ADD, true, 0);
  auto collageOutlinesDrawingLayerPtr = synthPtr->addDrawingLayer("6collage outlines",
                                                      synthPtr->getSize(), GL_RGBA16F, GL_CLAMP_TO_EDGE,
                                                      false, OF_BLENDMODE_ALPHA, false, 0);
  //  addFboConfigPtr(fboConfigPtrs, "cluster particles", fboClusterParticlesPtr, size, GL_RGBA16F, GL_CLAMP_TO_EDGE, false, OF_BLENDMODE_ALPHA, false, 0);
  auto majorLinesDrawingLayerPtr = synthPtr->addDrawingLayer("7major lines",
                                                    synthPtr->getSize(), GL_RGBA16F, GL_CLAMP_TO_EDGE,
                                                    true, OF_BLENDMODE_ALPHA, false, 0); // samples==4 is too much; 2 seems to push over the edge as well
  
  // Assign drawing surfaces to the Mods
  assignDrawingLayerPtrToMods(motionParticlesDrawingLayerPtr, {
    { particleFieldModPtr }
  });
  assignDrawingLayerPtrToMods(fluidDrawingLayerPtr, {
    { fluidModPtr },
    { drawPointsModPtr },
    { rawFluidPointsModPtr },
    { particleFieldModPtr } // ***************
  });
  assignDrawingLayerPtrToMods(fluidVelocitiesDrawingLayerPtr, {
    { fluidModPtr, FluidMod::VELOCITIES_LAYERPTR_NAME },
    { fluidRadialImpulseModPtr },
    { rawFluidRadialImpulseModPtr }
  });
  assignDrawingLayerPtrToMods(rawPointsDrawingLayerPtr, {
    { drawPointsModPtr },
    { smearModPtr },
    { sandLineModPtr }
  });
  assignDrawingLayerPtrToMods(collageDrawingLayerPtr, {
    { collageModPtr },
    { collageFadeModPtr }
  });
  assignDrawingLayerPtrToMods(collageOutlinesDrawingLayerPtr, {
    { collageModPtr, CollageMod::OUTLINE_LAYERPTR_NAME },
    { outlineFadeModPtr }
  });
  assignDrawingLayerPtrToMods(minorLinesDrawingLayerPtr, {
    { dividedAreaModPtr }
  });
  assignDrawingLayerPtrToMods(majorLinesDrawingLayerPtr, {
    { dividedAreaModPtr, DividedAreaMod::MAJOR_LINES_LAYERPTR_NAME }
  });

  // TODO: these aren't right are they?
  smearModPtr->receive(SmearMod::SINK_FIELD_2_FBO, fluidVelocitiesDrawingLayerPtr->fboPtr->getSource());
  particleFieldModPtr->receive(ParticleFieldMod::SINK_FIELD_2_FBO, fluidVelocitiesDrawingLayerPtr->fboPtr->getSource());
  
  synthPtr->configureGui();
  return synthPtr;
}
