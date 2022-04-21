#include <assert.h>
#include <math.h>
#include <signal.h>
#include <stdint.h>
#include <string.h>
#include <atomic>
#include "audio.h"

constexpr double kPI = 3.14159265359;
constexpr double kTAU = 2.0 * kPI;

// @TODO: Remove this global...
static std::atomic<bool> quit = false;

static double Clamp(double value, double min, double max) {
  if (value < min) return min;
  if (value > max) return max;
  return value;
}

static double Smoothstep(double edge0, double edge1, double value) {
  double t = Clamp((value - edge0) / (edge1 - edge0), 0.0f, 1.0f);
  return t * t * (3.0 - 2.0 * t);
}

static double EaseInOut(double u) {
  double ease_region = 0.3;
  return Smoothstep(0.0, ease_region, u) - Smoothstep(1.0 - ease_region, 1.0, u);
}

std::vector<Stereo> GenerateBeat(double sample_rate, double tempo) {
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

class Metronome {
private:
  AudioSystem audio;

public:
  int Run(uint32_t tempo = 120, uint32_t sample_rate = 44100);
};

int Metronome::Run(uint32_t tempo, uint32_t sample_rate) {
  if (!audio.Initialize(sample_rate)) return 1;

  auto InterruptHandler = [](int signal) { quit = true; };
  signal(SIGINT, InterruptHandler);

  auto samples = GenerateBeat(static_cast<double>(sample_rate), tempo);
  audio.Play(samples, sample_rate);

  while (!quit) {
    // Wait for the change...
    Sleep(100);
  }

  audio.Terminate();

  return 0;
}

void PrintUsage() {
  printf("pace is cli metronome\n\n");
  printf("USAGE:\n");
  printf("\tpace [OPTIONS]\n\n");
  printf("OPTIONS\n");

  printf("  -tempo int\n");
  printf("  \ttempo in beats per minute, range [60, 300]\n");
  printf("  -help\n");
  printf("  \tdispay this help text\n");
}

int main(int argc, const char* argv[]) {
  bool help = false;
  int tempo = 120;

  for (int i = 1; i < argc; ++i) {
    if (strcmp(argv[i], "-tempo") == 0 && i < (argc - 1)) {
      tempo = atoi(argv[++i]);
    } else if (strcmp(argv[i], "-help") == 0) {
      help = true;
    }
  }

  if (help) {
    PrintUsage();
    return 0;
  }

  constexpr int kMinTempo = 60;
  constexpr int kMaxTempo = 300;

  if (tempo < kMinTempo || tempo > kMaxTempo) {
    fprintf(stderr, "ERROR: invalid tempo '%d', must be between %d and %d\n\n", tempo, kMinTempo, kMaxTempo);
    return 0;
  }

  Metronome metronome;
  return metronome.Run(static_cast<uint32_t>(tempo));
}
