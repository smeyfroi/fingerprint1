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
    {"MinPitch", "50.0"},
    {"MaxPitch", "600.0"},//"1500.0"},
    {"MinRms", "0.0005"},
    {"MaxRms", MAX_RMS},
    {"MinComplexSpectralDifference", "200.0"},
    {"MaxComplexSpectralDifference", "1000.0"},
    {"MinSpectralCrest", "20.0"},
    {"MaxSpectralCrest", "350.0"},
    {"MinZeroCrossingRate", "5.0"},
    {"MaxZeroCrossingRate", "15.0"},
  }, MIC_DEVICE_NAME, RECORD_AUDIO, RECORDING_PATH, ROOT_SOURCE_MATERIAL_PATH));
  
  auto smearModPtr = synthPtr->addMod<SmearMod>("Smear Raw Points", {
    {"Translation", "0.0, 0.0"},
    {"MixNew", "0.7"},
    {"AlphaMultiplier", "0.997"},
    {"Field1Multiplier", "0.02"},
    {"Field1Bias", "0, 0"},
    {"Field2Multiplier", "0.01"},
    {"Field2Bias", "0, 0"},
  });

  auto audioPaletteModPtr = synthPtr->addMod<SomPaletteMod>("Palette Creator", {
    {"Iterations", "4000"},
  });
  
  auto clusterModPtr = synthPtr->addMod<ClusterMod>("Clusters", {
    {"maxSourcePoints", "600"},
    {"clusters", "7"},
  });
  
  auto polarClusterModPtr = synthPtr->addMod<ClusterMod>("Polar Clusters", {
    {"maxSourcePoints", "600"},
    {"clusters", "5"},
  });
  
  auto fluidModPtr = synthPtr->addMod<FluidMod>("Fluid", {
    {"dt", "0.008"},
    {"value:dissipation", "0.999"},
    {"velocity:dissipation", "0.998"},
    {"vorticity", "50.0"},
    {"value:iterations", "0.0"},
    {"velocity:iterations", "0.0"},
    {"pressure:iterations", "25.0"},
  });
  
  auto fluidRadialImpulseModPtr = synthPtr->addMod<FluidRadialImpulseMod>("Cluster Impulses", {
    {"ImpulseRadius", "0.05"},
    {"ImpulseStrength", "0.02"},
  });
  
  auto polarFluidRadialImpulseModPtr = synthPtr->addMod<FluidRadialImpulseMod>("Polar Cluster Impulses", {
    {"ImpulseRadius", "0.03"},
    {"ImpulseStrength", "0.01"},
  });
  
  auto clusterPointsModPtr = synthPtr->addMod<SoftCircleMod>("Cluster Points", {
    {"Radius Mean", "0.01"},
    {"Radius Variance", "1.0"},
    {"Radius Min", "0.0"},
    {"Radius Max", "0.07"},
    {"ColorMultiplier", "0.5"},
    {"AlphaMultiplier", "0.3"},
    {"Softness", "0.8"},
  });
  
  auto pitchRmsPointsModPtr = synthPtr->addMod<SoftCircleMod>("Raw Points", {
    {"Radius Mean", "0.003"},
    {"Radius Variance", "1.0"},
    {"Radius Min", "0.0"},
    {"Radius Max", "0.02"},
    {"ColorMultiplier", "0.4"},
    {"AlphaMultiplier", "0.6"},
    {"Softness", "0.6"},
  });
  
  auto polarPitchRmsPointsModPtr = synthPtr->addMod<SoftCircleMod>("Polar Raw Points", {
    {"Radius Mean", "0.003"},
    {"Radius Variance", "0.5"},
    {"Radius Min", "0.0"},
    {"Radius Max", "0.01"},
    {"Softness", "0.5"},
    {"AlphaMultiplier", "0.9"},
    {"ColorMultiplier", "0.4"},
  });
  
  connectSourceToSinks(audioDataSourceModPtr, {
    { AudioDataSourceMod::SOURCE_SPECTRAL_POINTS, {
      { audioPaletteModPtr, SomPaletteMod::SINK_VEC3 },
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
  });
  assignDrawingLayerPtrToMods(fadeDrawingLayerPtr, {
    { polarPitchRmsPointsModPtr },
  });

  // TODO: set up as DrawingLayers?
  smearModPtr->receive(SmearMod::SINK_FIELD_2_FBO, fluidVelocitiesDrawingLayerPtr->fboPtr->getSource());
  
  synthPtr->configureGui();
  return synthPtr;
}
