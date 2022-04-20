#pragma once

#include <Windows.h>
#include <wrl/client.h>
#include <xaudio2.h>
#include <stdint.h>
#include <vector>

using SoundSample = int16_t;

struct Stereo {
  SoundSample left;
  SoundSample right;
};

struct VoiceCallback : IXAudio2VoiceCallback {
  void __stdcall OnVoiceProcessingPassStart(UINT32 bytes_required) override;
  void __stdcall OnVoiceProcessingPassEnd() override;
  void __stdcall OnStreamEnd() override;
  void __stdcall OnBufferStart(void* buffer_context) override;
  void __stdcall OnBufferEnd(void* buffer_context) override;
  void __stdcall OnLoopEnd(void* buffer_context) override;
  void __stdcall OnVoiceError(void* buffer_context, HRESULT error) override;
};

class AudioSystem {
private:
  IXAudio2* xaudio = nullptr;
  IXAudio2MasteringVoice* mastering_voice = nullptr;
  IXAudio2SourceVoice* source_voice = nullptr;
  VoiceCallback voice_callback;
  WAVEFORMATEX format;
  XAUDIO2_BUFFER buffer;

  std::vector<Stereo> output;

public:
  bool Initialize(uint32_t sample_rate);
  void Terminate();
};
