#pragma once

#include <stdint.h>
#include "imgui.h"
#include "imgui_internal.h"

struct TempoContext {
  uint32_t shader;
  float tempo;
};

struct SubdivisionContext {
  uint32_t shader;
  int subdivision;
  bool selected;
  bool hovered;
  bool active;
};

namespace Widgets {
  IMGUI_API bool Beats(int* value, int min_value, int max_value);
  IMGUI_API bool Tempo(TempoContext* context, const char* label, ImFont* font, float width, float height);
  IMGUI_API bool Subdivision(SubdivisionContext* context, const char* label, float width, float height);
}
