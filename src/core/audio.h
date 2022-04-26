#pragma once

#include <Windows.h>
#include <xaudio2.h>
#include <stdint.h>
#include <mutex>
#include <thread>
#include <vector>
#include "signal.h"

using SoundSample = int16_t;

struct Stereo {
  SoundSample left;
  SoundSample right;
};

struct VoiceCallback : IXAudio2VoiceCallback {
  Signal buffer_end;

  VoiceCallback() = default;
  virtual ~VoiceCallback() = default;

  void __stdcall OnVoiceProcessingPassStart(UINT32 bytes_required) override { }
  void __stdcall OnVoiceProcessingPassEnd() override { }
  void __stdcall OnStreamEnd() override { }
  void __stdcall OnBufferStart(void* buffer_context) override { }
  void __stdcall OnBufferEnd(void* buffer_context) override;
  void __stdcall OnLoopEnd(void* buffer_context) override { }
  void __stdcall OnVoiceError(void* buffer_context, HRESULT error) override { }
};

struct SoundOutput {
  std::vector<Stereo> output_buffer;
  XAUDIO2_BUFFER buffer;
  IXAudio2SourceVoice* voice = nullptr;
  VoiceCallback callback;
  std::atomic<bool> quit = false;

  std::mutex mutex;
  std::vector<Stereo> samples;
  size_t sample_index = 0;
};

class AudioSystem {
private:
  IXAudio2* xaudio = nullptr;
  IXAudio2MasteringVoice* mastering_voice = nullptr;

  std::thread sound_output_thread;
  SoundOutput sound_output;

public:
  bool Initialize(uint32_t sample_rate);
  void Terminate();

  bool Play(const std::vector<Stereo>& samples);
  void Stop();
};
