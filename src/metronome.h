#pragma once

#include "audio.h"

class Metronome {
private:
  AudioSystem audio;

public:
  int Run(uint32_t tempo = 120, uint32_t sample_rate = 44100);
};
