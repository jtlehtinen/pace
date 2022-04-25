#include "application.h"
#include "imgui.h"
#include "imgui-knobs.h"
#include "widgets.h"
#include "ogl.h"

static const char* vertex_shader_source = R"(
#version 330 core

layout (location = 0) in vec2 Position;
layout (location = 1) in vec2 UV;
layout (location = 2) in vec4 Color;

uniform mat4 ProjMtx;

void main() {
  gl_Position = ProjMtx * vec4(Position.xy,0,1);
}
)";

static const char* fragment_shader_source = R"(
#version 330 core

uniform vec4 BBox;
uniform float Time;

layout (location = 0) out vec4 o_color;

vec2 getResolution() {
  return vec2(BBox.z - BBox.x, BBox.w - BBox.y);
}

vec2 getFragCoord() {
  return gl_FragCoord.xy - BBox.xy;
}

const float kPi = 3.1415926535;
const float kTau = 2.0 * kPi;

vec3 hue2rgb(float hue) {
	hue = fract(hue);
  float r = abs(hue * 6.0 - 3.0) - 1.0;
  float g = 2.0 - abs(hue * 6.0 - 2.0);
  float b = 2.0 - abs(hue * 6.0 - 4.0);
  return clamp(vec3(r, g, b), 0.0, 1.0);
}

float ring(float begin, float end, float smoothing, float value) {
  return smoothstep(begin, begin + smoothing, value) - smoothstep(end - smoothing, end, value);
}

void main() {
  vec2 resolution = getResolution();
  vec2 fragCoord = getFragCoord();

  vec2 p = fragCoord.xy / resolution.xy - 0.5;
  p.x *= resolution.x / resolution.y;

  float angle = atan(p.y, p.x) / kTau;
  float len = length(p);

  float r = ring(0.35, 0.42, 0.03, len);
  vec3 c = 2.0 * r * hue2rgb(angle + Time / 6.0);

  o_color = vec4(c, 1.0);
}
)";

constexpr int kMinTempo = 60;
constexpr int kMaxTempo = 300;

constexpr int kMinEmphasis = 1;
constexpr int kMaxEmphasis = 32;

Application::Application() {

}

Application::~Application() {

}

bool Application::Initialize() {
  if (!OGL::Initialize()) return false;

  shader_program = OGL::CreateProgram(vertex_shader_source, fragment_shader_source);

  return metronome.Initialize(44100);
}

void Application::Terminate() {
  OGL::DestroyProgram(shader_program);
  metronome.Terminate();
}

void Application::Render() {
  --update_countdown;

  #ifdef IMGUI_HAS_VIEWPORT
  ImGuiViewport* viewport = ImGui::GetMainViewport();
  ImGui::SetNextWindowPos(viewport->WorkPos);
  ImGui::SetNextWindowSize(viewport->WorkSize);
  ImGui::SetNextWindowViewport(viewport->ID);
  #else
  ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
  ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
  #endif

  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

  ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize;

  bool open = true;
  ImGui::Begin("window", &open, window_flags);

  UIState previous_state = state;

  ImGui::SliderInt("tempo", &state.tempo, kMinTempo, kMaxTempo);
  ImGui::NewLine();

  ImGui::Text("subdivision: "); ImGui::SameLine();
  ImGui::RadioButton("1", &state.subdivision, 1); ImGui::SameLine();
  ImGui::RadioButton("2", &state.subdivision, 2); ImGui::SameLine();
  ImGui::RadioButton("3", &state.subdivision, 3); ImGui::SameLine();
  ImGui::RadioButton("4", &state.subdivision, 4);
  ImGui::NewLine();

  ImGui::SliderInt("emphasis", &state.emphasis, kMinEmphasis, kMaxEmphasis);

  if (Widgets::Tempo(&shader_program, 384.0f, 384.0f)) {
    playing = !playing;
    if (playing) {
      Parameters params = {
        .sample_rate = 44100,
        .tempo = static_cast<uint32_t>(state.tempo),
        .emphasis = static_cast<uint32_t>(state.emphasis),
        .subdivision = static_cast<uint32_t>(state.subdivision),
      };
      metronome.Play(params);
    } else {
      metronome.Stop();
    }
  }

  if (state != previous_state && update_countdown < 0) {
    update_countdown = 30;
  }

  if (playing && update_countdown == 0) {
    metronome.Stop();

    Parameters params = {
      .sample_rate = 44100,
      .tempo = static_cast<uint32_t>(state.tempo),
      .emphasis = static_cast<uint32_t>(state.emphasis),
      .subdivision = static_cast<uint32_t>(state.subdivision),
    };
    metronome.Play(params);
  }

  ImGui::End();
  ImGui::PopStyleVar(2);
}
