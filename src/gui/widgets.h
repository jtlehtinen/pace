#pragma once

#include <stdint.h>
#include "imgui.h"
#include "imgui_internal.h"

struct TempoContext {
  uint32_t shader;
};

struct SubdivisionContext {
  uint32_t shader;
  int subdivision;
};

namespace Widgets {
  IMGUI_API bool Tempo(TempoContext* contet, const char* label, float width, float height);
  IMGUI_API bool Subdivision(SubdivisionContext* context, const char* label, float width, float height);
}
