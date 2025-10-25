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

  std::shared_ptr<AudioDataSourceMod> audioDataSourceModPtr = std::static_pointer_cast<AudioDataSourceMod>(synthPtr->addMod<AudioDataSourceMod>("Audio Source", {
    {"MinPitch", "50.0"}, // Tuning: manual variable. Has large effect on point positions etc
    {"MaxPitch", "800.0"},//"1500.0"}, // Tuning: manual variable. Has large effect on point positions etc
    {"MinRms", "0.0005"}, // Tuning: manual variable. Has large effect on point positions etc
    {"MaxRms", MAX_RMS}, // Tuning: manual variable. Has large effect on point positions etc
    {"MinComplexSpectralDifference", "200.0"}, // how this get set? pre-performance tuning?. Has large effect on palette etc
    {"MaxComplexSpectralDifference", "500.0"}, // how this get set? pre-performance tuning?. Has large effect on palette etc
    {"MinSpectralCrest", "20.0"}, // how this get set? pre-performance tuning?. Has large effect on palette etc
    {"MaxSpectralCrest", "100.0"}, // how this get set? pre-performance tuning?. Has large effect on palette etc
    {"MinZeroCrossingRate", "5.0"}, // how this get set? pre-performance tuning?. Has large effect on palette etc
    {"MaxZeroCrossingRate", "15.0"}, // how this get set? pre-performance tuning?. Has large effect on palette etc
  }, MIC_DEVICE_NAME, RECORD_AUDIO, RECORDING_PATH, ROOT_SOURCE_MATERIAL_PATH));
  
  auto smearModPtr = synthPtr->addMod<SmearMod>("Smear", {
    {"Translation", "0.0, 0.0"}, // could be variable but unlikely to be useful
    {"MixNew", "0.7"}, // 0.3 (slow) -> 0.9 (fast); could be variable. When this slows down, the multipliers become useful so they are coupled to some extent
    {"AlphaMultiplier", "0.997"}, // fades fast when lower; could be variable with limits
    {"Field1Multiplier", "0.02"}, // speed up smear effect; could be variable with limits
    {"Field1Bias", "0, 0"}, // not variable: depends on field. Do we have any 0-1 fields that would need this?
    {"Field2Multiplier", "0.01"}, // speed up smear effect; could be variable with limits
    {"Field2Bias", "0, 0"}, // not variable: depends on field. Do we have any 0-1 fields that would need this?
  });

  auto audioPaletteModPtr = synthPtr->addMod<SomPaletteMod>("Palette Creator", {
    {"Iterations", "2000"}, // not variable: set per performance? Lower settles quickly, which is useful for simple music
  });
  
  auto clusterModPtr = synthPtr->addMod<ClusterMod>("Clusters", {
    {"maxSourcePoints", "600"}, // not variable: set per performance?
    {"clusters", "7"}, // could be a manual variable (or a cumulative event trigger)
  });
  
  auto polarClusterModPtr = synthPtr->addMod<ClusterMod>("Polar Clusters", {
    {"maxSourcePoints", "600"}, // not variable: set per performance (?)
    {"clusters", "5"}, // large impact: could be a manual variable (or a cumulative event trigger)
  });
  
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
  
  auto polarFluidRadialImpulseModPtr = synthPtr->addMod<FluidRadialImpulseMod>("Polar Cluster Impulses", {
    {"ImpulseRadius", "0.1"}, // as above
    {"ImpulseStrength", "0.03"}, // as above
  });
  
  auto clusterPointsModPtr = synthPtr->addMod<SoftCircleMod>("Cluster Points", {
    {"Radius Mean", "0.01"}, // could be variable
    {"Radius Variance", "1.0"}, // could be variable
    {"Radius Min", "0.0"}, // not variable
    {"Radius Max", "0.05"}, // not variable
    {"ColorMultiplier", "0.5"}, // could be variable
    {"AlphaMultiplier", "0.3"}, // could be variable
    {"Softness", "0.8"}, // could be variable
  });
  
  auto pitchRmsPointsModPtr = synthPtr->addMod<SoftCircleMod>("Pitch RMS Points", {
    {"Radius Mean", "0.005"},
    {"Radius Variance", "1.0"},
    {"Radius Min", "0.0"},
    {"Radius Max", "0.05"},
    {"ColorMultiplier", "0.4"},
    {"AlphaMultiplier", "0.6"},
    {"Softness", "0.6"},
  });
  
  auto polarPitchRmsPointsModPtr = synthPtr->addMod<SoftCircleMod>("Polar Pitch RMS Points", {
    {"Radius Mean", "0.004"},
    {"Radius Variance", "0.5"},
    {"Radius Min", "0.0"},
    {"Radius Max", "0.05"},
    {"Softness", "0.5"},
    {"AlphaMultiplier", "0.9"},
    {"ColorMultiplier", "0.4"},
  });
  
  auto polarSpectralPointsModPtr = synthPtr->addMod<SoftCircleMod>("Polar Spectral Points", {
    {"Radius Mean", "0.01"},
    {"Radius Variance", "0.5"},
    {"Radius Min", "0.0"},
    {"Radius Max", "0.02"},
    {"Softness", "1.0"},
    {"AlphaMultiplier", "0.2"},
    {"ColorMultiplier", "1.0"},
  });
  
  connectSourceToSinks(audioDataSourceModPtr, {
    { AudioDataSourceMod::SOURCE_SPECTRAL_3D_POINTS, {
      { audioPaletteModPtr, SomPaletteMod::SINK_VEC3 },
    }},
    { AudioDataSourceMod::SOURCE_SPECTRAL_2D_POINTS, {
      { polarSpectralPointsModPtr, SoftCircleMod::SINK_POINTS },
    }},
    { AudioDataSourceMod::SOURCE_PITCH_RMS_POINTS, {
      { clusterModPtr, ClusterMod::SINK_VEC2 },
      { fluidRadialImpulseModPtr, FluidRadialImpulseMod::SINK_POINTS },
      { pitchRmsPointsModPtr, SoftCircleMod::SINK_POINTS },
    }},
    { AudioDataSourceMod::SOURCE_POLAR_PITCH_RMS_POINTS, {
      { polarClusterModPtr, ClusterMod::SINK_VEC2 },
      { polarFluidRadialImpulseModPtr, FluidRadialImpulseMod::SINK_POINTS },
      { polarPitchRmsPointsModPtr, SoftCircleMod::SINK_POINTS },
    }},
    { AudioDataSourceMod::SOURCE_ONSET1, {
//      { clusterModPtr, ClusterMod::SINK_CHANGE_CLUSTER_NUM },
//      { smearModPtr, SmearMod::SINK_CHANGE_LAYER },
//      { polarPitchRmsPointsModPtr, SoftCircleMod::SINK_CHANGE_LAYER },
//      { clusterPointsModPtr, SoftCircleMod::SINK_CHANGE_LAYER },
//      { pitchRmsPointsModPtr, SoftCircleMod::SINK_CHANGE_LAYER },
    }},
//    { AudioDataSourceMod::SOURCE_TIMBRE_CHANGE, {
//      { audioPaletteModPtr, SomPaletteMod::SINK_SWITCH_PALETTE }
//    }},
//    { AudioDataSourceMod::SOURCE_PITCH_CHANGE, {
//    }},
  });
  connectSourceToSinks(audioPaletteModPtr, {
    { SomPaletteMod::SOURCE_FIELD, {
      { smearModPtr, SmearMod::SINK_FIELD_1_FBO },
    }},
    { SomPaletteMod::SOURCE_RANDOM_VEC4, {
      { clusterPointsModPtr, SoftCircleMod::SINK_POINT_COLOR },
    }},
    { SomPaletteMod::SOURCE_RANDOM_DARK_VEC4, {
      { pitchRmsPointsModPtr, SoftCircleMod::SINK_POINT_COLOR }
    }},
    { SomPaletteMod::SOURCE_RANDOM_LIGHT_VEC4, {
      { polarPitchRmsPointsModPtr, SoftCircleMod::SINK_POINT_COLOR },
    }},
    { SomPaletteMod::SOURCE_DARKEST_VEC4, {
      { synthPtr, Synth::SINK_BACKGROUND_COLOR }
    }},
  });
  connectSourceToSinks(clusterModPtr, {
    { ClusterMod::SOURCE_CLUSTER_CENTRE_VEC2, {
      { clusterPointsModPtr, SoftCircleMod::SINK_POINTS },
      { fluidRadialImpulseModPtr, FluidRadialImpulseMod::SINK_POINTS },
    }}
  });
  connectSourceToSinks(polarClusterModPtr, {
    { ClusterMod::SOURCE_CLUSTER_CENTRE_VEC2, {
      { clusterPointsModPtr, SoftCircleMod::SINK_POINTS },
//      { fluidRadialImpulseModPtr, FluidRadialImpulseMod::SINK_POINTS },
    }}
  });

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
//    { pitchRmsPointsModPtr },
  });
  assignDrawingLayerPtrToMods(fluidVelocitiesDrawingLayerPtr, {
    { fluidModPtr, FluidMod::VELOCITIES_LAYERPTR_NAME },
    { fluidRadialImpulseModPtr },
    { polarFluidRadialImpulseModPtr },
  });
  assignDrawingLayerPtrToMods(smearedDrawingLayerPtr, {
    { smearModPtr },
//    { clusterPointsModPtr },
//    { fluidRadialImpulseModPtr },
    { pitchRmsPointsModPtr },
    { polarSpectralPointsModPtr },
  });
  assignDrawingLayerPtrToMods(fadeDrawingLayerPtr, {
    { polarPitchRmsPointsModPtr },
  });

  // TODO: set up as DrawingLayers?
  smearModPtr->receive(SmearMod::SINK_FIELD_2_FBO, fluidVelocitiesDrawingLayerPtr->fboPtr->getSource());
  
  synthPtr->configureGui();
  return synthPtr;
}
