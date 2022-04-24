#pragma once

#include "audio.h"

namespace Beat {
  std::vector<Stereo> Generate(double sample_rate, double tempo, uint32_t subdivision, uint32_t emphasis);
}
