#pragma once

#include <Windows.h>
#include <wrl/client.h>
#include <xaudio2.h>
#include <stdint.h>
#include <thread>
#include <vector>
#include <atomic>

using SoundSample = int16_t;

struct Stereo {
  SoundSample left;
  SoundSample right;
};

struct VoiceCallback : IXAudio2VoiceCallback {
  HANDLE buffer_end_event = INVALID_HANDLE_VALUE;

  VoiceCallback() {
    buffer_end_event = CreateEventEx(nullptr, nullptr, 0, EVENT_MODIFY_STATE | SYNCHRONIZE);
  }

  virtual ~VoiceCallback() {
    CloseHandle(buffer_end_event);
  }

  void __stdcall OnVoiceProcessingPassStart(UINT32 bytes_required) override { }
  void __stdcall OnVoiceProcessingPassEnd() override { }
  void __stdcall OnStreamEnd() override { }
  void __stdcall OnBufferStart(void* buffer_context) override { }
  void __stdcall OnBufferEnd(void* buffer_context) override;
  void __stdcall OnLoopEnd(void* buffer_context) override { }
  void __stdcall OnVoiceError(void* buffer_context, HRESULT error) override { }
};

struct SineWave {
  float frequency;
  float amplitude;
  float t;
};

// @TODO: Thread safety of XAudio2 objects...
struct SoundThreadContext {
  std::atomic<bool> quit;
  VoiceCallback* voice_callback = nullptr;

  IXAudio2SourceVoice* source_voice;
  XAUDIO2_BUFFER* buffer;
  std::vector<Stereo>* samples;

  SineWave sine_wave;
};

class AudioSystem {
private:
  IXAudio2* xaudio = nullptr;
  IXAudio2MasteringVoice* mastering_voice = nullptr;
  IXAudio2SourceVoice* source_voice = nullptr;
  VoiceCallback voice_callback;
  WAVEFORMATEX format;

  XAUDIO2_BUFFER output_buffer;
  std::vector<Stereo> output_samples;

  float master_volume = 0.1f;

  // @TODO: Maybe fill sound buffers from the main thread?
  SoundThreadContext sound_thread_context;
  std::thread sound_thread;

public:
  bool Initialize(uint32_t sample_rate);
  void Terminate();

  float GetMasterVolume() const { return master_volume; }
  void SetMasterVolume(float master_volume) { this->master_volume = master_volume; }
};
