#pragma once

#include "core/metronome.h"

struct UIState {
  int tempo = 120;
  int subdivision = 1;
  int emphasis = 4;

  bool operator ==(UIState rhs) const;
  bool operator !=(UIState rhs) const;
};

inline bool UIState::operator ==(UIState rhs) const {
  return this->tempo == rhs.tempo &&
    this->subdivision == rhs.subdivision &&
    this->emphasis == rhs.emphasis;
}

inline bool UIState::operator !=(UIState rhs) const {
  return !(*this == rhs);
}

class Application {
  private:
    UIState state;
    Metronome metronome;

    uint32_t tempo_shader = 0;
    uint32_t subdivision_shader = 0;

  public:
    Application();
    ~Application();

    Application(const Application& other) = delete;
    void operator=(const Application& rhs) = delete;

    bool Initialize();
    void Terminate();
    void Render();
};
