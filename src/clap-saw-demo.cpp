#include "clap-saw-demo.h"
#include <algorithm>

ClapSawDemo::ClapSawDemo() {
  buildParameterDescriptions();
}

void ClapSawDemo::setSampleRate(double sr) {
  // AudioContext is configured by the wrapper, so we just update voice DSP
  for (auto& voice : voiceDSP) {
    // Any samplerate-dependent setup
  }
}

void ClapSawDemo::processAudioContext() {
  // AudioContext has already processed events and voice management.
  // We just read voice states and generate audio.
  activeVoiceCount = 0;
  audioContext->outputs[0] = ml::DSPVector{0.0f};
  audioContext->outputs[1] = ml::DSPVector{0.0f};

  // Process all voices - the gate multiplication naturally silences inactive ones
  // Limit to our voice array size to prevent crashes
  const int maxVoices = std::min(static_cast<int>(voiceDSP.size()), audioContext->getInputPolyphony());

  ml::DSPVector totalOutput{0.0f};

  for (int v = 0; v < maxVoices; ++v) {
    auto& voice = const_cast<ml::EventsToSignals::Voice&>(audioContext->getInputVoice(v));

    ml::DSPVector voiceOutput = processVoice(v, voice);

    // Track activity using DSPVector sum (SIMD-friendly)
    float voiceLevel = ml::sum(voiceOutput * voiceOutput);
    if (voiceLevel > 0.0f) {
      activeVoiceCount++;
    }

    totalOutput += voiceOutput;
  }

  // Apply gain parameter and voice normalization
  float masterGain = getRealFloatParam("gain");

  // Normalize by voice count to prevent clipping when multiple voices play
  float voiceNormalization = (activeVoiceCount > 0) ? (1.0f / std::sqrt(activeVoiceCount)) : 1.0f;

  // Apply moderate gain reduction (can increase once testing confirms we're not exploding speakers)
  float debugGainReduction = 0.3f;
  float totalGain = debugGainReduction * voiceNormalization * masterGain;

  // Apply final gain staging
  audioContext->outputs[0] = totalOutput * ml::DSPVector(totalGain);  // Mono to stereo
  audioContext->outputs[1] = totalOutput * ml::DSPVector(totalGain);
}

ml::DSPVector ClapSawDemo::processVoice(int voiceIndex, ml::EventsToSignals::Voice& voice) {
  // Bounds check to prevent crashes
  if (voiceIndex < 0 || voiceIndex >= voiceDSP.size()) {
    return ml::DSPVector{0.0f};
  }

  // Get voice control signals provided by EventsToSignals
  const ml::DSPVector vPitch = voice.outputs.row(ml::kPitch);  // MIDI note with pitch bend, gliding
  const ml::DSPVector vGate = voice.outputs.row(ml::kGate);    // MIDI velocity
  const ml::DSPVector vMod = voice.outputs.row(ml::kMod);      // Mod wheel, aftertouch

  const float sr = audioContext->getSampleRate();

  // Convert MIDI pitch to Hz using standard formula: 440 * 2^((note-69)/12)
  const ml::DSPVector vPitchOffset = vPitch - ml::DSPVector(69.0f);
  const ml::DSPVector vPitchRatio = pow(ml::DSPVector(2.0f), vPitchOffset * ml::DSPVector(1.0f/12.0f));
  const ml::DSPVector vFreqHz = ml::DSPVector(440.0f) * vPitchRatio;
  const ml::DSPVector vFreqNorm = vFreqHz / ml::DSPVector(sr);
  const ml::DSPVector vOscillator = voiceDSP[voiceIndex].sawOscillator(vFreqNorm);

  voiceDSP[voiceIndex].mLoPass._coeffs = ml::Lopass::makeCoeffs(12000.0f / sr, 1.414f);
  const ml::DSPVector vFiltered = voiceDSP[voiceIndex].mLoPass(vOscillator);
  const ml::DSPVector vOutput = vFiltered * vGate;

  return vOutput;
}

void ClapSawDemo::buildParameterDescriptions() {
  ml::ParameterDescriptionList params;

  params.push_back(std::make_unique<ml::ParameterDescription>(ml::WithValues{
    {"name", "gain"},
    {"range", {0.0f, 1.0f}},
    {"default", 0.5f},
    {"units", ""}
  }));

  params.push_back(std::make_unique<ml::ParameterDescription>(ml::WithValues{
    {"name", "f0"},
    {"range", {10.0f, 10000.0f}},
    {"default", 1000.0f},
    {"units", "Hz"}
  }));

  params.push_back(std::make_unique<ml::ParameterDescription>(ml::WithValues{
    {"name", "Q"},
    {"range", {0.01f, 10.0f}},
    {"default", 1.4f},
    {"units", ""}
  }));

  buildParams(params);

  // Set default parameter values after building
  setDefaultParams();
}
