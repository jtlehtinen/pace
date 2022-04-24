#include "beat.h"
#include "frequency.h"
#include <assert.h>

namespace {

  constexpr double kPI = 3.14159265359;
  constexpr double kTAU = 2.0 * kPI;

  double Clamp(double value, double min, double max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
  }

  double Smoothstep(double edge0, double edge1, double value) {
    double t = Clamp((value - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    return t * t * (3.0 - 2.0 * t);
  }

  double EaseInOut(double u) {
    double ease_region = 0.3;
    return Smoothstep(0.0, ease_region, u) - Smoothstep(1.0 - ease_region, 1.0, u);
  }
}

std::vector<Stereo> Beat::Generate(double sample_rate, double tempo, uint32_t subdivision, uint32_t emphasis) {
  size_t quarter_notes = emphasis;

  constexpr double kSecondsInMinute = 60.0;
  double length_in_seconds = quarter_notes * kSecondsInMinute / tempo;
  size_t length_in_samples = static_cast<size_t>(sample_rate * length_in_seconds + 0.5);

  std::vector<Stereo> result(length_in_samples, Stereo{.left=0, .right=0});

  const size_t ticks = quarter_notes * subdivision;

  std::vector<size_t> tick_samples(ticks);
  for (size_t i = 0; i < tick_samples.size(); ++i) {
    tick_samples[i] = length_in_samples * i / tick_samples.size();
  }

  constexpr size_t tick_length_in_samples = 2048;

  for (size_t i = 0; i < tick_samples.size(); ++i) {
    bool emp = ((i / subdivision) % emphasis) == 0;
    bool quarter = (i % subdivision) == 0;

    double frequency = (emp && quarter) ? Frequency::C6 : quarter ? Frequency::C5 : Frequency::C4;

    double samples_period = sample_rate / frequency;

    double t = 0.0;
    double tstep = kTAU / samples_period;

    for (size_t j = 0; j < tick_length_in_samples; ++j) {
      double amplitude = EaseInOut(static_cast<double>(j) / (static_cast<double>(tick_length_in_samples - 1)));

      int16_t value = static_cast<int16_t>(amplitude * static_cast<double>(INT16_MAX) * sin(t));
      result[j + tick_samples[i]].left += value;
      result[j + tick_samples[i]].right += value;
      t += tstep;
    }
  }

  return result;
}
