#include "application.h"
#include "imgui.h"
#include "imgui-knobs.h"
#include <math.h>

namespace {

  bool PlayStopButton(bool playing) {
    bool clicked = ImGui::InvisibleButton("play", ImVec2(50.0f, 50.0f));

    ImVec2 min = ImGui::GetItemRectMin();
    ImVec2 max = ImGui::GetItemRectMax();
    bool hovered = ImGui::IsItemHovered();
    bool active = ImGui::IsItemActive();

    auto draw = ImGui::GetWindowDrawList();

    auto Lightness = [](bool active, bool hovered) {
      if (active) return 1.0f;
      if (hovered) return 0.9f;
      return 0.7f;
    };

    auto FrameColor = [](bool active, bool hovered) {
      if (active) return ImGui::GetColorU32(ImGuiCol_ButtonActive);
      if (hovered) return ImGui::GetColorU32(ImGuiCol_ButtonHovered);
      return ImGui::GetColorU32(ImGuiCol_Button);
    };

    if (playing) {
      float width = max.x - min.x;
      float height = max.y - min.y;
      float hpad = width * 0.2f;
      float vpad = height * 0.2f;

      auto lightness = Lightness(active, hovered);
      auto fill_color = ImVec4(lightness * 0.8f, lightness * 0.17f, lightness * 0.17f, 1.0f);

      draw->AddRect(min, max, FrameColor(active, hovered), 5.0f, 0, 3.0f);
      draw->AddRectFilled(ImVec2(min.x + hpad, min.y + vpad), ImVec2(max.x - hpad, max.y - vpad), ImGui::GetColorU32(fill_color), 5.0f);
    } else {
      float width = max.x - min.x;
      float height = max.y - min.y;
      float hpad = width * 0.15f;
      float vpad = height * 0.15f;

      // @NOTE: Height of a equilateral triangle = side*sqrt(3)/2
      float triheight = (max.x - min.x - 2.0f * hpad) * sqrtf(3.0f) * 0.5f;
      hpad = (width - triheight) * 0.5f;

      float minx = min.x + hpad;
      float maxx = max.x - hpad;
      float miny = min.y + vpad;
      float maxy = max.y - vpad;
      float midy = (miny + maxy) * 0.5f;

      auto p0 = ImVec2(minx, miny);
      auto p1 = ImVec2(maxx, midy);
      auto p2 = ImVec2(minx, maxy);

      auto lightness = Lightness(active, hovered);
      auto fill_color = ImVec4(lightness * 0.17f, lightness * 0.8f, lightness * 0.44f, 1.0f);

      draw->AddRect(min, max, FrameColor(active, hovered), 5.0f, 0, 3.0f);
      draw->AddTriangleFilled(p0, p1, p2, ImGui::GetColorU32(fill_color));
    }

    return clicked;
  };

}

void UIState::FixInvalidState() {
  float min_tempo = static_cast<float>(kMetronomeMinTempo);
  float max_tempo = static_cast<float>(kMetronomeMaxTempo);

  float min_beats = static_cast<float>(kMetronomeMinBeats);
  float max_beats = static_cast<float>(kMetronomeMaxBeats);

  float min_subdivision = static_cast<float>(kMetronomeMinSubdivision);
  float max_subdivision = static_cast<float>(kMetronomeMaxSubdivision);


  if (tempo < min_tempo) tempo = min_tempo;
  if (tempo > max_tempo) tempo = max_tempo;

  if (beats < min_beats) beats = min_beats;
  if (beats > max_beats) beats = max_beats;

  if (subdivision < min_subdivision) subdivision = min_subdivision;
  if (subdivision > max_subdivision) subdivision = max_subdivision;
}

MetronomeParameters UIState::ToMetronomeParameters() const {
  return MetronomeParameters{
    .tempo = static_cast<uint32_t>(roundf(tempo)),
    .beats = static_cast<uint32_t>(roundf(beats)),
    .subdivision = static_cast<uint32_t>(roundf(subdivision)),
  };
}

bool Application::Initialize() {
  return metronome.Initialize(44100);
}

void Application::Terminate() {
  metronome.Terminate();
}

void Application::Render(uint32_t window_width, uint32_t window_height) {
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

  auto before = state.ToMetronomeParameters();

  // @TODO: ImGuiKnobs can set the parameter value out of the given [min, max] range.

  const float hspace = 30.0f;
  ImGuiKnobs::WiperDotKnob("tempo", &state.tempo, kMetronomeMinTempo, kMetronomeMaxTempo, "%.0f", 50.0f);
  ImGui::SameLine(0.0f, hspace);

  ImGuiKnobs::WiperDotKnob("beats", &state.beats, kMetronomeMinBeats, kMetronomeMaxBeats, "%.0f", 50.0f);
  ImGui::SameLine(0.0f, hspace);

  ImGuiKnobs::WiperDotKnob("subdiv", &state.subdivision, kMetronomeMinSubdivision, kMetronomeMaxSubdivision, "%.0f", 50.0f);
  ImGui::SameLine(0.0f, hspace);
  ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 40.0f);

  state.FixInvalidState();

  if (PlayStopButton(state.playing)) {
    state.playing = !state.playing;
    if (state.playing) {
      metronome.Play(state.ToMetronomeParameters());
    } else {
      metronome.Stop();
    }
  }

  auto after = state.ToMetronomeParameters();
  if (after != before && state.playing) {
    metronome.Play(state.ToMetronomeParameters());
  }

  ImGui::End();
  ImGui::PopStyleVar(2);
}
