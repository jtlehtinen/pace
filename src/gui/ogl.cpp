#include "ogl.h"
#include <stdio.h>

namespace {

  bool CheckShaderCompileStatus(GLuint shader) {
    GLint status = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    return status == GL_TRUE;
  }

  bool CheckProgramLinkStatus(GLuint program) {
    GLint status = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    return status == GL_TRUE;
  }

  void PrintShaderInfoLog(GLuint shader) {
    char buffer[2048] = { };
    glGetShaderInfoLog(shader, sizeof(buffer), nullptr, buffer);
    printf("%s\n", buffer);
  }

  void PrintProgramInfoLog(GLuint program) {
    char buffer[2048] = { };
    glGetProgramInfoLog(program, sizeof(buffer), nullptr, buffer);
    printf("%s\n", buffer);
  }

  uint32_t CreateShader(const char* source, GLenum type) {
    auto s = glCreateShader(type);
    glShaderSource(s, 1, &source, nullptr);
    glCompileShader(s);

    if (!CheckShaderCompileStatus(s)) {
      PrintShaderInfoLog(s);
      glDeleteShader(s);
      return OGL::kInvalidShader;
    }

    return s;
  }
}

bool OGL::Initialize() {
  return gladLoadGL() == 1;
}

uint32_t OGL::CreateProgram(const char* vertex_shader_source, const char* fragment_shader_source) {
  auto vs = CreateShader(vertex_shader_source, GL_VERTEX_SHADER);
  auto fs = CreateShader(fragment_shader_source, GL_FRAGMENT_SHADER);
  auto p = glCreateProgram();

  glAttachShader(p, vs);
  glAttachShader(p, fs);
  glLinkProgram(p);

  glDeleteShader(vs);
  glDeleteShader(fs);

  if (!CheckProgramLinkStatus(p)) {
    PrintProgramInfoLog(p);
    glDeleteProgram(p);
    return OGL::kInvalidShaderProgram;
  }

  return p;
}

void OGL::DestroyProgram(uint32_t program) {
  if (program != kInvalidShaderProgram) {
    glDeleteProgram(program);
  }
}
