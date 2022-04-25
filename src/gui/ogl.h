#pragma once

#include <stdint.h>
#include "glad.h"

namespace OGL {
  bool Initialize();

  constexpr uint32_t kInvalidShader = 0;
  constexpr uint32_t kInvalidShaderProgram = 0;

  uint32_t CreateProgram(const char* vertex_shader_source, const char* fragment_shader_source);
  void DestroyProgram(uint32_t program);
}


