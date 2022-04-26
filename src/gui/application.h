#pragma once

#include "core/metronome.h"

struct UIState {
  float tempo = 120.0f;
  float beats = 4.0f;
  float subdivision = 1.0f;
  bool playing = false;

  void FixInvalidState(); // @TODO: ImGuiKnobs can set values out of [min, max] range...
  MetronomeParameters ToMetronomeParameters() const;
};

class Application {
  private:
    UIState state;
    Metronome metronome;

  public:
    Application() = default;
    ~Application() = default;

    Application(const Application& other) = delete;
    void operator=(const Application& rhs) = delete;

    bool Initialize();
    void Terminate();
    void Render(uint32_t window_width, uint32_t window_height);
};
