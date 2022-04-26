#include "application.h"
#include "imgui.h"
#include "imgui-knobs.h"
#include "widgets.h"
#include "ogl.h"
#include "shaders.h"

constexpr int kMinTempo = 60;
constexpr int kMaxTempo = 300;

constexpr int kMinEmphasis = 1;
constexpr int kMaxEmphasis = 32;

Application::Application() {

}

Application::~Application() {

}

bool Application::Initialize(ImGuiFonts fonts) {
  if (!OGL::Initialize()) return false;

  this->fonts = fonts;

  tempo_shader = OGL::CreateProgram(Shaders::Tempo::vertex_shader_source, Shaders::Tempo::fragment_shader_source);
  subdivision_shader = OGL::CreateProgram(Shaders::Subdivision::vertex_shader_source, Shaders::Subdivision::fragment_shader_source);
  background_shader = OGL::CreateProgram(Shaders::Background::vertex_shader_source, Shaders::Background::fragment_shader_source);
  glGenVertexArrays(1, &dummy_vao);

  return metronome.Initialize(44100);
}

void Application::Terminate() {
  OGL::DestroyProgram(subdivision_shader);
  OGL::DestroyProgram(tempo_shader);
  metronome.Terminate();
}

void Application::Render(uint32_t window_width, uint32_t window_height) {
  glUseProgram(background_shader);
  glUniform2f(0, static_cast<GLfloat>(window_width), static_cast<GLfloat>(window_height));
  glBindVertexArray(dummy_vao);
  glDrawArrays(GL_TRIANGLES, 0, 3);
  glBindVertexArray(0);
  glUseProgram(0);

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

  ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground;

  bool open = true;
  ImGui::Begin("window", &open, window_flags);

  Widgets::Beats(&state.beats, 1, 16);

  static TempoContext tempo_ctx;
  tempo_ctx = TempoContext{.shader = tempo_shader, .tempo = state.tempo};
  Widgets::Tempo(&tempo_ctx, "tempo", fonts.large, 384.0f, 384.0f);
  state.tempo = tempo_ctx.tempo;

  static SubdivisionContext subdiv_ctx[4];
  subdiv_ctx[0] = SubdivisionContext{.shader = subdivision_shader, .subdivision = 1, .selected = state.subdivision == 1};
  subdiv_ctx[1] = SubdivisionContext{.shader = subdivision_shader, .subdivision = 2, .selected = state.subdivision == 2};
  subdiv_ctx[2] = SubdivisionContext{.shader = subdivision_shader, .subdivision = 3, .selected = state.subdivision == 3};
  subdiv_ctx[3] = SubdivisionContext{.shader = subdivision_shader, .subdivision = 4, .selected = state.subdivision == 4};

  if (Widgets::Subdivision(&subdiv_ctx[0], "subdivision1", 85.0f, 80.0f)) state.subdivision = 1;
  ImGui::SameLine();

  if (Widgets::Subdivision(&subdiv_ctx[1], "subdivision2", 85.0f, 80.0f)) state.subdivision = 2;
  ImGui::SameLine();

  if (Widgets::Subdivision(&subdiv_ctx[2], "subdivision3", 85.0f, 80.0f)) state.subdivision = 3;
  ImGui::SameLine();

  if (Widgets::Subdivision(&subdiv_ctx[3], "subdivision4", 85.0f, 80.0f)) state.subdivision = 4;

  // @TODO: emphasis

  ImGui::End();
  ImGui::PopStyleVar(2);
}
