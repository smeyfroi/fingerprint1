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
  auto synthPtr = std::make_shared<Synth>("SynthSoftCircle", ModConfig {
    {"Back Color", "0.0, 0.0, 0.0, 1.0"},
  }, START_PAUSED, size);

  // >>> AUDIO, CLUSTERS AND PALETTE
  std::shared_ptr<AudioDataSourceMod> audioDataSourceModPtr = std::static_pointer_cast<AudioDataSourceMod>(synthPtr->addMod<AudioDataSourceMod>("AudioSource", {
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
  
  auto audioPaletteModPtr = synthPtr->addMod<SomPaletteMod>("AudioPalette", {
    {"Iterations", "1000"}, // not variable: set per performance? Lower settles quickly, which is useful for simple music
  });
  
  auto clusterModPtr = synthPtr->addMod<ClusterMod>("Clusters", {
    {"Max Source Points", "600"}, // not variable: set per performance?
    {"Clusters", "7"}, // could be a manual variable (or a cumulative event trigger)
  });
  // <<< AUDIO, CLUSTERS AND PALETTE
  
  // >>> LAYER PROCESSES
  auto fluidModPtr = synthPtr->addMod<FluidMod>("Fluid", {
    {"dt", "0.002"}, // could be a manual variable but changes need to be small for stability
    {"Value Dissipation", "0.999"}, // could be a manual variable but changes need to be small for stability
    {"Velocity Dissipation", "0.998"}, // could be a manual variable but changes need to be small for stability
    {"Vorticity", "50.0"}, // not an obvious visible effect so perhaps set per performance
    {"Value Iterations", "0.0"}, // could be variable to 1.0 or 2.0, which smooths things out noticeably
    {"Velocity Iterations", "0.0"}, // could tie it to above to smooth things out?
    {"Pressure Iterations", "25.0"}, // not variable
  });

  auto fluidRadialImpulseModPtr = synthPtr->addMod<FluidRadialImpulseMod>("ClusterImpulses", {
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
  
  auto fadeModPtr = synthPtr->addMod<FadeMod>("Fade", {
    {"Alpha", "0.01"}, // could be variable
  });
  // <<< LAYER PROCESSES
  
  // >>> POINT SIZE PROCESSES
  auto smallRmsScalarModPtr = synthPtr->addMod<MultiplyAddMod>("SmallRmsScalar", {
    {"Mul", "0.002"}, // could be variable
    {"Add", "0.005"}, // could be variable
  });
  auto midRmsScalarModPtr = synthPtr->addMod<MultiplyAddMod>("MidRmsScalar", {
    {"Mul", "0.005"}, // could be variable
    {"Add", "0.01"}, // could be variable
  });
  auto largeRmsScalarModPtr = synthPtr->addMod<MultiplyAddMod>("LargeRmsScalar", {
    {"Mul", "0.01"}, // could be variable
    {"Add", "0.02"}, // could be variable
  });
  // <<< POINT SIZE PROCESSES
  
  // >>> POINTS
  auto clusterPointsModPtr = synthPtr->addMod<SoftCircleMod>("ClusterPoints", {
    {"Color Multiplier", "0.5"}, // could be variable
    {"Alpha Multiplier", "0.2"}, // could be variable
    {"Softness", "0.8"}, // could be variable
  });
  
  auto pitchRmsPointsModPtr = synthPtr->addMod<SoftCircleMod>("PitchRmsPoints", {
    {"Color Multiplier", "0.4"},
    {"Alpha Multiplier", "0.5"},
    {"Softness", "0.6"},
  });
  
  auto polarSpectralPointsModPtr = synthPtr->addMod<SoftCircleMod>("PolarSpectralPoints", {
    {"Color Multiplier", "1.0"},
    {"Alpha Multiplier", "0.2"},
    {"Softness", "1.0"},
  });
  // <<< POINTS
  
  // >>> CONNECTIONS
  synthPtr->addConnections(R"(
    # AUDIO, CLUSTERS AND PALETTE
    AudioSource.spectral3dPoints -> AudioPalette.vec3
    AudioSource.pitchRmsPoints -> Clusters.vec2
    AudioPalette.darkest -> .backgroundColor
  )");
  
  synthPtr->addConnections(R"(
    # POINT SIZE PROCESSES
    AudioSource.rmsScalar -> SmallRmsScalar.float
    AudioSource.rmsScalar -> MidRmsScalar.float
    AudioSource.rmsScalar -> LargeRmsScalar.float
  )");
  
  synthPtr->addConnections(R"(
    # LAYER PROCESSES
    Clusters.clusterCentreVec2 -> ClusterImpulses.points
    AudioSource.pitchRmsPoints -> ClusterImpulses.points
    LargeRmsScalar.float -> ClusterImpulses.impulseRadius
    AudioPalette.field -> Smear.field1Fbo
  )");
  
  synthPtr->addConnections(R"(
    # POINTS
    AudioSource.polarSpectral2dPoints -> PolarSpectralPoints.points
    AudioSource.pitchRmsPoints -> PitchRmsPoints.points
    Clusters.clusterCentreVec2 -> ClusterPoints.points
    
    SmallRmsScalar.float -> PolarSpectralPoints.radius
    MidRmsScalar.float -> PitchRmsPoints.radius
    LargeRmsScalar.float -> ClusterPoints.radius

    AudioPalette.darkest -> PolarSpectralPoints.color
    AudioPalette.randomDark -> PitchRmsPoints.color
    AudioPalette.randomLight -> ClusterPoints.color
  )");
  // <<< CONNECTIONS

  // Make drawing layers
  auto fluidDrawingLayerPtr = synthPtr->addDrawingLayer("1fluid",
                                            synthPtr->getSize() / 8.0, GL_RGBA32F, GL_REPEAT,
                                            false, OF_BLENDMODE_ADD, true, 0);
  auto fluidVelocitiesDrawingLayerPtr = synthPtr->addDrawingLayer("fluidVelocities",
                                                      synthPtr->getSize() / 8.0, GL_RGB32F, GL_REPEAT,
                                                      false, OF_BLENDMODE_DISABLED, false, 0, false); // not drawn
  auto smearedDrawingLayerPtr = synthPtr->addDrawingLayer("2smear",
                                                synthPtr->getSize(), GL_RGBA32F, GL_REPEAT,
                                                false, OF_BLENDMODE_ADD, true, 0);
  // FIXME: are all drawing layers GL_REPEAT and never GL_CLAMP_TO_EDGE?
  auto fadeDrawingLayerPtr = synthPtr->addDrawingLayer("3fade",
                                            synthPtr->getSize(), GL_RGBA32F, GL_REPEAT,
                                            false, OF_BLENDMODE_ADD, true, 0);
  
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
    { fadeModPtr },
    { polarSpectralPointsModPtr },
  });

  // TODO: set up as DrawingLayers?
  smearModPtr->receive(SmearMod::SINK_FIELD_2_FBO, fluidVelocitiesDrawingLayerPtr->fboPtr->getSource());
  
  return synthPtr;
}
