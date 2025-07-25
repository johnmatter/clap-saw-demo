#pragma once

#include "madronalib.h"           // madronalib core (AudioContext with EventsToSignals)
#include <clap/helpers/plugin.hh>  // CLAP helper framework
#include <clap/clap.h>             // CLAP core

class ClapSawDemo : public ml::SignalProcessor {
private:
  // the AudioContext's EventsToSignals handles state
  ml::AudioContext* audioContext = nullptr;  // Set by wrapper

  // Per-voice DSP components
  struct VoiceDSP {
    ml::SawGen sawOscillator;
  };
  std::array<VoiceDSP, 16> voiceDSP;

  // Simple voice activity tracking for CLAP
  int activeVoiceCount = 0;

public:
  ClapSawDemo();
  ~ClapSawDemo() override = default;

  // SignalProcessor interface
  void setSampleRate(double sr);
  void buildParameterDescriptions();

  void processAudioContext();
  void setAudioContext(ml::AudioContext* ctx) { audioContext = ctx; }

  // Voice activity for CLAP sleep/continue
  bool hasActiveVoices() const { return activeVoiceCount > 0; }

private:
  // Helper methods go here
  ml::DSPVector processVoice(int voiceIndex, ml::EventsToSignals::Voice& voice);
};

