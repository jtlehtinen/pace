#include "application.h"
#include "imgui.h"

constexpr int kMinTempo = 120;
constexpr int kMaxTempo = 300;

constexpr int kMinEmphasis = 1;
constexpr int kMaxEmphasis = 32;

Application::Application() {

}

Application::~Application() {

}

bool Application::Init() {
  return true;
}

void Application::Render() {
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

  ImGui::SliderInt("tempo", &tempo, kMinTempo, kMaxTempo);
  ImGui::NewLine();

  ImGui::Text("subvdivision: "); ImGui::SameLine();
  ImGui::RadioButton("1", &subdivision, 1); ImGui::SameLine();
  ImGui::RadioButton("2", &subdivision, 2); ImGui::SameLine();
  ImGui::RadioButton("3", &subdivision, 3); ImGui::SameLine();
  ImGui::RadioButton("4", &subdivision, 4);
  ImGui::NewLine();

  ImGui::SliderInt("emphasis", &emphasis, kMinEmphasis, kMaxEmphasis);

  ImGui::End();
  ImGui::PopStyleVar(2);
}
