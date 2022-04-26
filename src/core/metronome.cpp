#include "metronome.h"
#include "beat.h"

bool Metronome::Initialize(uint32_t sample_rate) {
  this->sample_rate = sample_rate;
  return audio.Initialize(sample_rate);
}

void Metronome::Terminate() {
  audio.Terminate();
}

void Metronome::Play(MetronomeParameters params) {
  auto samples = Beat::Generate(sample_rate, params.tempo, params.subdivision, params.beats);
  audio.Play(samples);
}

void Metronome::Stop() {
  audio.Stop();
}
