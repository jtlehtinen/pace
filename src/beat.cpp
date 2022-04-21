#include "beat.h"

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

std::vector<Stereo> Beat::Generate(double sample_rate, double tempo) {
  // @NOTE: Figure out the minimum sound buffer size
  // in samples we can loop to playback this beat.

  constexpr double kSecondsInMinute = 60.0;
  double length_in_seconds = 4.0 * kSecondsInMinute / tempo;
  size_t length_in_samples = static_cast<size_t>(sample_rate * length_in_seconds + 0.5);

  std::vector<Stereo> result(length_in_samples, Stereo{.left=0, .right=0});

  size_t first_sample[4];
  first_sample[0] = 0;
  first_sample[1] = length_in_samples / 4;
  first_sample[2] = length_in_samples / 2;
  first_sample[3] = length_in_samples * 3 / 4;

  constexpr size_t tick_length_in_samples = 2048;

  constexpr double first_beat_frequency = 523.2511; // C5
  constexpr double other_beat_frequency = 261.625565; // C4

  const double first_beat_samples_per_period = sample_rate / first_beat_frequency; // tau
  const double other_beat_samples_per_period = sample_rate / other_beat_frequency; // tau

  for (size_t i = 0; i < 4; ++i) {
    double samples_per_period = (i == 0 ? first_beat_samples_per_period : other_beat_samples_per_period);

    double t = 0.0;
    double tstep = kTAU / samples_per_period;

    for (size_t j = 0; j < tick_length_in_samples; ++j) {
      double amplitude = EaseInOut(static_cast<double>(j) / (static_cast<double>(tick_length_in_samples - 1)));

      int16_t value = static_cast<int16_t>(amplitude * static_cast<double>(INT16_MAX) * sin(t));
      result[j + first_sample[i]].left = value;
      result[j + first_sample[i]].right = value;
      t += tstep;
    }
  }

  return result;
}
