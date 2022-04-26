#pragma once

#include "audio.h"

constexpr uint32_t kMetronomeMinTempo = 10;
constexpr uint32_t kMetronomeMaxTempo = 300;

constexpr uint32_t kMetronomeMinBeats = 1;
constexpr uint32_t kMetronomeMaxBeats = 16;

constexpr uint32_t kMetronomeMinSubdivision = 1;
constexpr uint32_t kMetronomeMaxSubdivision = 4;

struct MetronomeParameters {
  uint32_t tempo = 120;
  uint32_t beats = 4;
  uint32_t subdivision = 1;

  bool Valid() const {
    return
      (tempo >= kMetronomeMinTempo && tempo <= kMetronomeMaxTempo) &&
      (beats >= kMetronomeMinBeats && beats <= kMetronomeMaxBeats) &&
      (subdivision >= kMetronomeMinSubdivision && subdivision <= kMetronomeMaxSubdivision);
  }

  bool operator ==(MetronomeParameters other) const;
  bool operator !=(MetronomeParameters other) const;
};

inline bool MetronomeParameters::operator ==(MetronomeParameters other) const {
  return
    (tempo == other.tempo) &&
    (beats == other.beats) &&
    (subdivision == other.subdivision);
}

inline bool MetronomeParameters::operator !=(MetronomeParameters other) const {
  return !(*this == other);
}

class Metronome {
private:
  AudioSystem audio;
  uint32_t sample_rate = 0;

public:
  bool Initialize(uint32_t sample_rate = 44100);
  void Terminate();

  void Play(MetronomeParameters params);
  void Stop();
};
