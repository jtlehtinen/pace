#include "application.h"
#include "imgui.h"

constexpr int kMinTempo = 60;
constexpr int kMaxTempo = 300;

constexpr int kMinEmphasis = 1;
constexpr int kMaxEmphasis = 32;

Application::Application() {

}

Application::~Application() {

}

bool Application::Initialize() {
  return metronome.Initialize(44100);
}

void Application::Terminate() {
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

  ImGui::Text("subvdivision: "); ImGui::SameLine();
  ImGui::RadioButton("1", &state.subdivision, 1); ImGui::SameLine();
  ImGui::RadioButton("2", &state.subdivision, 2); ImGui::SameLine();
  ImGui::RadioButton("3", &state.subdivision, 3); ImGui::SameLine();
  ImGui::RadioButton("4", &state.subdivision, 4);
  ImGui::NewLine();

  ImGui::SliderInt("emphasis", &state.emphasis, kMinEmphasis, kMaxEmphasis);

  if (ImGui::Button(playing ? "Stop" : "Play")) {
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
