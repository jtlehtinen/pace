#include "metronome.h"
#include "beat.h"

bool Metronome::Initialize(uint32_t sample_rate) {
  return audio.Initialize(sample_rate);
}

void Metronome::Terminate() {
  audio.Terminate();
}

void Metronome::Play(Parameters params) {
  auto samples = Beat::Generate(params.sample_rate, params.tempo, params.subdivision, params.emphasis);
  audio.Play(samples, params.sample_rate);
}

void Metronome::Stop() {
  audio.Stop();
}
