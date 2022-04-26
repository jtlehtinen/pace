#pragma once

#include "core/metronome.h"

struct ImFont;

struct ImGuiFonts {
  ImFont* normal;
  ImFont* large;
};

struct UIState {
  float tempo = 120.0f;
  int beats = 4;
  int subdivision = 1;

  bool operator ==(UIState rhs) const;
  bool operator !=(UIState rhs) const;
};

inline bool UIState::operator ==(UIState rhs) const {
  return this->tempo == rhs.tempo &&
    this->subdivision == rhs.subdivision &&
    this->beats == rhs.beats;
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
    uint32_t background_shader = 0;
    uint32_t dummy_vao = 0;

    ImGuiFonts fonts;

  public:
    Application();
    ~Application();

    Application(const Application& other) = delete;
    void operator=(const Application& rhs) = delete;

    bool Initialize(ImGuiFonts fonts);
    void Terminate();
    void Render(uint32_t window_width, uint32_t window_height);
};
