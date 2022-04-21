#pragma once

#include "audio.h"

namespace Beat {
  std::vector<Stereo> Generate(double sample_rate, double tempo);
}
