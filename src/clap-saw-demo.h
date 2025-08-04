#pragma once

#include "madronalib.h"           // madronalib core (AudioContext with EventsToSignals)
#include <clap/helpers/plugin.hh>  // CLAP helper framework
#include <clap/clap.h>             // CLAP core
#include <functional>              // For std::function

#ifdef HAS_GUI
class ClapSawDemoGUI;
#endif

class ClapSawDemo : public ml::SignalProcessor {
private:
  // the AudioContext's EventsToSignals handles state
  ml::AudioContext* audioContext = nullptr;  // Set by wrapper

  // Per-voice DSP components
  struct VoiceDSP {
    ml::SawGen sawOscillator;
    ml::Lopass mLoPass;
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



  // CLAP parameter interface required by CLAPPluginWrapper
  uint32_t getParameterCount() const { return _params.descriptions.size(); }
  const ml::ParameterTree& getParameterTree() const { return _params; }

  // CLAP logging interface
  void setHostLogCallback(std::function<void(int, const char*)> callback);
  void logToHost(int severity, const char* message);

  // CLAP parameter flush interface (for GUI->Host sync)
  void setHostParameterFlushCallback(std::function<void()> callback);
  void requestHostParameterFlush();

private:
  // Helper methods go here
  ml::DSPVector processVoice(int voiceIndex, ml::EventsToSignals::Voice& voice);
  
  // Logging callback - set by CLAPPluginWrapper
  std::function<void(int, const char*)> hostLogCallback;
  
  // Parameter flush callback - set by CLAPPluginWrapper
  std::function<void()> hostParameterFlushCallback;
};
