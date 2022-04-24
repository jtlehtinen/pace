#pragma once

#include "audio.h"

struct Parameters {
  uint32_t sample_rate = 44100;
  uint32_t tempo = 120;
  uint32_t emphasis = 4;
  uint32_t subdivision = 1;
};

class Metronome {
private:
  AudioSystem audio;

public:
  bool Initialize(uint32_t sample_rate);
  void Terminate();

  void Play(Parameters params);
  void Stop();
};