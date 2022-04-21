#pragma once

#include <Windows.h>
#include <xaudio2.h>
#include <stdint.h>
#include <vector>

using SoundSample = int16_t;

struct Stereo {
  SoundSample left;
  SoundSample right;
};

using Sound = uint32_t;
constexpr Sound kInvalidSound = ~(0u);

struct SoundBuffer {
  std::vector<Stereo> samples;
  IXAudio2SourceVoice* voice = nullptr;
  XAUDIO2_BUFFER buffer;
};

class AudioSystem {
private:
  IXAudio2* xaudio = nullptr;
  IXAudio2MasteringVoice* mastering_voice = nullptr;
  SoundBuffer* buffer = nullptr;

public:
  bool Initialize(uint32_t sample_rate);
  void Terminate();

  bool Play(const std::vector<Stereo>& samples, size_t sample_rate);
};
