//
//  ofSynthSoftCircle.cpp
//  fingerprint1
//
//  Created by Steve Meyfroidt on 24/10/2025.
//

// Test Synth to explore SoftCircleMod, SomPaletteMod, SmearMod and FluidMod

#include "ofApp.h"

using namespace ofxMarkSynth;

std::shared_ptr<Synth> createSynthSoftCircle(glm::vec2 size) {
  auto synthPtr = std::make_shared<Synth>("SynthSoftCircle", ModConfig {}, START_PAUSED, size);

  // >>> AUDIO, CLUSTERS AND PALETTE
  std::shared_ptr<AudioDataSourceMod> audioDataSourceModPtr = std::static_pointer_cast<AudioDataSourceMod>(synthPtr->addMod<AudioDataSourceMod>("Audio Source", {
    {"MinPitch", "50.0"}, // Tuning: manual variable. Has large effect on point positions etc
    {"MaxPitch", "800.0"},//"1500.0"}, // Tuning: manual variable. Has large effect on point positions etc
    {"MinRms", "0.0005"}, // Tuning: manual variable. Has large effect on point positions etc
    {"MaxRms", MAX_RMS}, // Tuning: manual variable. Has large effect on point positions etc
    {"MinComplexSpectralDifference", "200.0"}, // how this get set? pre-performance tuning?. Has large effect on palette etc
    {"MaxComplexSpectralDifference", "500.0"}, // how this get set? pre-performance tuning?. Has large effect on palette etc
    {"MinSpectralCrest", "20.0"}, // how this get set? pre-performance tuning?. Has large effect on palette etc
    {"MaxSpectralCrest", "200.0"}, // how this get set? pre-performance tuning?. Has large effect on palette etc
    {"MinZeroCrossingRate", "5.0"}, // how this get set? pre-performance tuning?. Has large effect on palette etc
    {"MaxZeroCrossingRate", "15.0"}, // how this get set? pre-performance tuning?. Has large effect on palette etc
  }, MIC_DEVICE_NAME, RECORD_AUDIO, RECORDING_PATH, ROOT_SOURCE_MATERIAL_PATH));
  
  auto audioPaletteModPtr = synthPtr->addMod<SomPaletteMod>("Palette Creator", {
    {"Iterations", "2000"}, // not variable: set per performance? Lower settles quickly, which is useful for simple music
  });
  
  auto clusterModPtr = synthPtr->addMod<ClusterMod>("Clusters", {
    {"maxSourcePoints", "600"}, // not variable: set per performance?
    {"clusters", "7"}, // could be a manual variable (or a cumulative event trigger)
  });
  // <<< AUDIO, CLUSTERS AND PALETTE
  
  // >>> LAYER PROCESSES
  auto fluidModPtr = synthPtr->addMod<FluidMod>("Fluid", {
    {"dt", "0.008"}, // could be a manual variable but changes need to be small for stability
    {"value:dissipation", "0.999"}, // could be a manual variable but changes need to be small for stability
    {"velocity:dissipation", "0.998"}, // could be a manual variable but changes need to be small for stability
    {"vorticity", "50.0"}, // not an obvious visible effect so perhaps set per performance
    {"value:iterations", "0.0"}, // could be variable to 1.0 or 2.0, which smooths things out noticeably
    {"velocity:iterations", "0.0"}, // could tie it to above to smooth things out?
    {"pressure:iterations", "25.0"}, // not variable
  });

  auto fluidRadialImpulseModPtr = synthPtr->addMod<FluidRadialImpulseMod>("Cluster Impulses", {
    {"ImpulseRadius", "0.15"}, // could be variable
    {"ImpulseStrength", "0.015"}, // could be variable
  });
  
  auto smearModPtr = synthPtr->addMod<SmearMod>("Smear", {
    {"Translation", "0.0, 0.0"}, // could be variable but unlikely to be useful
    {"MixNew", "0.7"}, // 0.3 (slow) -> 0.9 (fast); could be variable. When this slows down, the multipliers become useful so they are coupled to some extent
    {"AlphaMultiplier", "0.997"}, // fades fast when lower; could be variable with limits
    {"Field1Multiplier", "0.02"}, // speed up smear effect; could be variable with limits
    {"Field1Bias", "0, 0"}, // not variable: depends on field. Do we have any 0-1 fields that would need this?
    {"Field2Multiplier", "0.01"}, // speed up smear effect; could be variable with limits
    {"Field2Bias", "0, 0"}, // not variable: depends on field. Do we have any 0-1 fields that would need this?
  });
  // <<< LAYER PROCESSES
  
  // >>> POINTS
  auto clusterPointsModPtr = synthPtr->addMod<SoftCircleMod>("Cluster Points", {
    {"Color Multiplier", "0.5"}, // could be variable
    {"Alpha Multiplier", "0.2"}, // could be variable
    {"Softness", "0.8"}, // could be variable
  });
  
  auto pitchRmsPointsModPtr = synthPtr->addMod<SoftCircleMod>("Pitch RMS Points", {
    {"Color Multiplier", "0.4"},
    {"Alpha Multiplier", "0.5"},
    {"Softness", "0.6"},
  });
  
  auto polarSpectralPointsModPtr = synthPtr->addMod<SoftCircleMod>("Polar Spectral Points", {
    {"Color Multiplier", "1.0"},
    {"Alpha Multiplier", "0.2"},
    {"Softness", "1.0"},
  });
  // <<< POINTS
  
  // >>> AUDIO, CLUSTERS AND PALETTE
  connectSourceToSinks(audioDataSourceModPtr, {
    { AudioDataSourceMod::SOURCE_SPECTRAL_3D_POINTS, {
      { audioPaletteModPtr, SomPaletteMod::SINK_VEC3 },
    }},
    { AudioDataSourceMod::SOURCE_PITCH_RMS_POINTS, {
      { clusterModPtr, ClusterMod::SINK_VEC2 },
    }},
  });
  connectSourceToSinks(audioPaletteModPtr, {
    { SomPaletteMod::SOURCE_DARKEST_VEC4, {
      { synthPtr, Synth::SINK_BACKGROUND_COLOR }
    }},
  });
  // <<< AUDIO, CLUSTERS AND PALETTE
  
  // >>> LAYER PROCESSES
  connectSourceToSinks(clusterModPtr, {
    { ClusterMod::SOURCE_CLUSTER_CENTRE_VEC2, {
      { fluidRadialImpulseModPtr, FluidRadialImpulseMod::SINK_POINTS },
    }}
  });
  connectSourceToSinks(audioDataSourceModPtr, {
    { AudioDataSourceMod::SOURCE_PITCH_RMS_POINTS, {
      { fluidRadialImpulseModPtr, FluidRadialImpulseMod::SINK_POINTS },
    }},
    { AudioDataSourceMod::SOURCE_RMS_SCALAR, {
      { fluidRadialImpulseModPtr, FluidRadialImpulseMod::SINK_IMPULSE_RADIUS }, // TODO: MulAdd
    }},
  });
  connectSourceToSinks(audioPaletteModPtr, {
    { SomPaletteMod::SOURCE_FIELD, {
      { smearModPtr, SmearMod::SINK_FIELD_1_FBO },
    }},
  });
  // <<< LAYER PROCESSES
  
  // >>> POINTS
  connectSourceToSinks(audioDataSourceModPtr, {
    { AudioDataSourceMod::SOURCE_POLAR_SPECTRAL_2D_POINTS, {
      { polarSpectralPointsModPtr, SoftCircleMod::SINK_POINTS },
    }},
    { AudioDataSourceMod::SOURCE_PITCH_RMS_POINTS, {
      { pitchRmsPointsModPtr, SoftCircleMod::SINK_POINTS },
    }},
  });
  connectSourceToSinks(clusterModPtr, {
    { ClusterMod::SOURCE_CLUSTER_CENTRE_VEC2, {
      { clusterPointsModPtr, SoftCircleMod::SINK_POINTS },
    }}
  });
  connectSourceToSinks(audioDataSourceModPtr, {
    { AudioDataSourceMod::SOURCE_SPECTRAL_CREST_SCALAR, {
      { polarSpectralPointsModPtr, SoftCircleMod::SINK_POINT_RADIUS }, // TODO: MulAdd
    }},
    { AudioDataSourceMod::SOURCE_ZERO_CROSSING_RATE_SCALAR, {
      { pitchRmsPointsModPtr, SoftCircleMod::SINK_POINT_RADIUS }, // TODO: MulAdd
    }},
    { AudioDataSourceMod::SOURCE_RMS_SCALAR, {
      { clusterPointsModPtr, SoftCircleMod::SINK_POINT_RADIUS }, // TODO: MulAdd
    }},
  });
  connectSourceToSinks(audioPaletteModPtr, {
    { SomPaletteMod::SOURCE_DARKEST_VEC4, {
      { polarSpectralPointsModPtr, SoftCircleMod::SINK_POINT_COLOR }
    }},
    { SomPaletteMod::SOURCE_RANDOM_DARK_VEC4, {
      { pitchRmsPointsModPtr, SoftCircleMod::SINK_POINT_COLOR }
    }},
    { SomPaletteMod::SOURCE_RANDOM_LIGHT_VEC4, {
      { clusterPointsModPtr, SoftCircleMod::SINK_POINT_COLOR },
    }},
  });
  // <<< POINTS

  // ***************************************************************************
  // TODO: need params for the layer fades
  // ***************************************************************************
  
  // Make drawing layers
  auto fluidDrawingLayerPtr = synthPtr->addDrawingLayer("1fluid",
                                            synthPtr->getSize() / 8.0, GL_RGBA16F, GL_REPEAT,
                                            0.0f, OF_BLENDMODE_ADD, true, 0);
  auto fluidVelocitiesDrawingLayerPtr = synthPtr->addDrawingLayer("fluidVelocities",
                                                      synthPtr->getSize() / 8.0, GL_RGB16F, GL_REPEAT,
                                                      0.0f, OF_BLENDMODE_DISABLED, false, 0, false); // not drawn
  auto smearedDrawingLayerPtr = synthPtr->addDrawingLayer("2smear",
                                                synthPtr->getSize(), GL_RGBA16F, GL_REPEAT,
                                                0.0f, OF_BLENDMODE_ADD, true, 0);
  // FIXME: are all drawing layers GL_REPEAT and never GL_CLAMP_TO_EDGE?
  auto fadeDrawingLayerPtr = synthPtr->addDrawingLayer("3fade",
                                            synthPtr->getSize(), GL_RGBA16F, GL_REPEAT,
                                            0.0005f, OF_BLENDMODE_ADD, true, 0);
  
  // Assign drawing layers to the Mods
  assignDrawingLayerPtrToMods(fluidDrawingLayerPtr, {
    { fluidModPtr },
    { clusterPointsModPtr },
  });
  assignDrawingLayerPtrToMods(fluidVelocitiesDrawingLayerPtr, {
    { fluidModPtr, FluidMod::VELOCITIES_LAYERPTR_NAME },
    { fluidRadialImpulseModPtr },
  });
  assignDrawingLayerPtrToMods(smearedDrawingLayerPtr, {
    { smearModPtr },
    { pitchRmsPointsModPtr },
  });
  assignDrawingLayerPtrToMods(fadeDrawingLayerPtr, {
    { polarSpectralPointsModPtr },
  });

  // TODO: set up as DrawingLayers?
  smearModPtr->receive(SmearMod::SINK_FIELD_2_FBO, fluidVelocitiesDrawingLayerPtr->fboPtr->getSource());
  
  synthPtr->configureGui();
  return synthPtr;
}
