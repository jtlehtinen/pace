#include "widgets.h"
#include "ogl.h"
#include <string>

constexpr float kPi = 3.14159265359f;

namespace {

  float Clamp(float value, float min, float max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
  }

  float Length(ImVec2 v) {
    return sqrtf(v.x * v.x + v.y * v.y);
  }

  ImVec2 Normalize(ImVec2 v) {
    float l = Length(v);
    return ImVec2(v.x / l, v.y / l);
  }

  float Dot(ImVec2 a, ImVec2 b) {
    return a.x * b.x + a.y * b.y;
  }

  ImVec2 Perp(ImVec2 v) {
    return ImVec2(-v.y, v.x);
  }

}

bool Widgets::Beats(int* value, int min_value, int max_value) {
  auto CenteredText = [](const char* text) {
    auto x = (ImGui::GetWindowWidth() - ImGui::CalcTextSize(text).x) * 0.5f;
    ImGui::SetCursorPosX(x);
    ImGui::Text(text);
  };

  auto PushDisabled = [](bool condition) {
    if (condition) {
      ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
      ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
    }
  };

  auto PopDisabled = [](bool condition) {
    if (condition) {
      ImGui::PopItemFlag();
      ImGui::PopStyleVar();
    }
  };

  float cx = ImGui::GetWindowWidth() * 0.5f;
  float line_height = ImGui::GetTextLineHeight();

  float button_dim = line_height;
  float button_distance_to_center = 44.0f;

  int new_value = *value;

  CenteredText("BEATS");


  ImGui::SetCursorPosX(cx - button_dim - button_distance_to_center);
  PushDisabled(*value <= min_value);
  if (ImGui::ArrowButtonEx("beat-left", ImGuiDir_Left, ImVec2(button_dim, button_dim))) {
    new_value -= 1;
  }
  PopDisabled(*value <= min_value);
  ImGui::SameLine();


  char buf[32] = { };
  sprintf_s(buf, sizeof(buf), "%d", *value);
  CenteredText(buf);
  ImGui::SameLine();


  ImGui::SetCursorPosX(cx + button_distance_to_center);
  PushDisabled(*value >= max_value);
  if (ImGui::ArrowButtonEx("beat-right", ImGuiDir_Right, ImVec2(button_dim, button_dim))) {
    new_value += 1;
  }
  PopDisabled(*value >= max_value);


  if (new_value != *value && new_value >= min_value && new_value <= max_value) {
    *value = new_value;
    return true;
  } else {
    return false;
  }
}

bool Widgets::Tempo(TempoContext* context, const char* label, ImFont* font, float width, float height) {
  // @TODO: Mess, clean it...
  // @TODO: Interaction...

  ImVec2 size(width, height);
  bool ret = ImGui::InvisibleButton(label, size);

  bool hovered = ImGui::IsItemHovered();
  bool active = ImGui::IsItemActive();

  ImVec2 min = ImGui::GetItemRectMin();
  ImVec2 max = ImGui::GetItemRectMax();

  if (active) {
    ImVec2 center = ImVec2((max.x + min.x) / 2.0f, (max.y + min.y) / 2.0f);
    ImVec2 drag = ImGui::GetMouseDragDelta();
    ImVec2 position = ImGui::GetMousePos();

    if (drag.x != 0.0f || drag.y != 0.0f) {
      float start_dx = position.x - drag.x - center.x;
      float start_dy = center.y - position.y + drag.y;

      float stop_dx = position.x - center.x;
      float stop_dy = center.y - position.y;

      auto start = ImVec2(start_dx, start_dy);
      auto stop = ImVec2(stop_dx, stop_dy);
      if (Length(start) != 0.0f && Length(stop) != 0.0f) {
        start = Normalize(start);
        stop = Normalize(stop);

        bool clockwise = Dot(stop, Perp(start)) < 0.0f;

        float angle = acosf(Clamp(Dot(stop, start), 0.0f, 1.0f));
        float delta = angle * 10.0f;
        if (!clockwise) delta *= -1.0f;

        context->tempo += delta;

        // @TODO: Pass in min and max...
        if (context->tempo < 60.0f) context->tempo = 60.0f;
        if (context->tempo > 300.0f) context->tempo = 300.0f;

        ImGui::ResetMouseDragDelta();
      }
    }
  }

  ImDrawList* draw_list = ImGui::GetWindowDrawList();
  draw_list->PushClipRect(min, max);

  auto callback = [](const ImDrawList* draw_list, const ImDrawCmd* draw_cmd) {
    auto ctx = (TempoContext*)draw_cmd->UserCallbackData;

    ImDrawData* draw_data = ImGui::GetDrawData();

    ImVec2 clip_off = draw_data->DisplayPos;         // (0,0) unless using multi-viewports
    ImVec2 clip_scale = draw_data->FramebufferScale; // (1,1) unless using retina display which are often (2,2)

    ImVec2 clip_min((draw_cmd->ClipRect.x - clip_off.x) * clip_scale.x, (draw_cmd->ClipRect.y - clip_off.y) * clip_scale.y);
    ImVec2 clip_max((draw_cmd->ClipRect.z - clip_off.x) * clip_scale.x, (draw_cmd->ClipRect.w - clip_off.y) * clip_scale.y);

    float L = draw_data->DisplayPos.x;
    float R = draw_data->DisplayPos.x + draw_data->DisplaySize.x;
    float T = draw_data->DisplayPos.y;
    float B = draw_data->DisplayPos.y + draw_data->DisplaySize.y;

    const float ortho_projection[4][4] = {
        { 2.0f/(R-L),   0.0f,         0.0f,   0.0f },
        { 0.0f,         2.0f/(T-B),   0.0f,   0.0f },
        { 0.0f,         0.0f,        -1.0f,   0.0f },
        { (R+L)/(L-R),  (T+B)/(B-T),  0.0f,   1.0f },
    };

    float time = (float)ImGui::GetTime();

    glUseProgram(ctx->shader);
    glUniformMatrix4fv(glGetUniformLocation(ctx->shader, "ProjMtx"), 1, GL_FALSE, &ortho_projection[0][0]);
    glUniform1f(glGetUniformLocation(ctx->shader, "Time"), time);

    float minx = clip_min.x;
    float miny = draw_data->DisplaySize.y - clip_max.y;
    float maxx = clip_max.x;
    float maxy = draw_data->DisplaySize.y - clip_min.y;

    glUniform4f(glGetUniformLocation(ctx->shader, "BBox"), minx, miny, maxx, maxy);
  };

  draw_list->AddCallback(callback, context);
  draw_list->AddRectFilled(min, max, 0xFF000000);
  draw_list->PopClipRect();
  draw_list->AddCallback(ImDrawCallback_ResetRenderState, nullptr);

  static char buf[32];
  sprintf(buf, "%d", static_cast<int>(context->tempo));

  ImU32 col32text = ImGui::GetColorU32(ImGuiCol_Text);

  ImGui::PushFont(font);
  ImVec2 text_size = ImGui::CalcTextSize(buf, buf + strlen(buf));
  ImVec2 text_position = ImVec2((min.x + max.x) / 2, (min.y + max.y) / 2);
  text_position.x -= text_size.x / 2.0f;
  text_position.y -= text_size.y / 2.0f;
  draw_list->AddText(text_position, col32text, buf, buf + strlen(buf));
  ImGui::PopFont();

  return ret;
}

bool Widgets::Subdivision(SubdivisionContext* context, const char* label, float width, float height) {
  ImVec2 size(width, height);
  bool ret = ImGui::InvisibleButton(label, size);
  context->hovered = ImGui::IsItemHovered();
  context->active = ImGui::IsItemActive();

  ImVec2 min = ImGui::GetItemRectMin();
  ImVec2 max = ImGui::GetItemRectMax();

  ImDrawList* draw_list = ImGui::GetWindowDrawList();
  draw_list->PushClipRect(min, max);


  auto callback = [](const ImDrawList* draw_list, const ImDrawCmd* draw_cmd) {
    auto ctx = (SubdivisionContext*)draw_cmd->UserCallbackData;

    ImDrawData* draw_data = ImGui::GetDrawData();

    ImVec2 clip_off = draw_data->DisplayPos;         // (0,0) unless using multi-viewports
    ImVec2 clip_scale = draw_data->FramebufferScale; // (1,1) unless using retina display which are often (2,2)

    ImVec2 clip_min((draw_cmd->ClipRect.x - clip_off.x) * clip_scale.x, (draw_cmd->ClipRect.y - clip_off.y) * clip_scale.y);
    ImVec2 clip_max((draw_cmd->ClipRect.z - clip_off.x) * clip_scale.x, (draw_cmd->ClipRect.w - clip_off.y) * clip_scale.y);

    float L = draw_data->DisplayPos.x;
    float R = draw_data->DisplayPos.x + draw_data->DisplaySize.x;
    float T = draw_data->DisplayPos.y;
    float B = draw_data->DisplayPos.y + draw_data->DisplaySize.y;

    const float ortho_projection[4][4] = {
        { 2.0f/(R-L),   0.0f,         0.0f,   0.0f },
        { 0.0f,         2.0f/(T-B),   0.0f,   0.0f },
        { 0.0f,         0.0f,        -1.0f,   0.0f },
        { (R+L)/(L-R),  (T+B)/(B-T),  0.0f,   1.0f },
    };

    float time = (float)ImGui::GetTime();

    glUseProgram(ctx->shader);
    glUniformMatrix4fv(glGetUniformLocation(ctx->shader, "ProjMtx"), 1, GL_FALSE, &ortho_projection[0][0]);
    glUniform1f(glGetUniformLocation(ctx->shader, "Time"), time);

    float minx = clip_min.x;
    float miny = draw_data->DisplaySize.y - clip_max.y;
    float maxx = clip_max.x;
    float maxy = draw_data->DisplaySize.y - clip_min.y;

    glUniform4f(glGetUniformLocation(ctx->shader, "BBox"), minx, miny, maxx, maxy);
    glUniform1i(glGetUniformLocation(ctx->shader, "subdiv"), ctx->subdivision - 1);

    int state = ctx->active ? 2 : ctx->hovered ? 1 : 0;
    glUniform1i(glGetUniformLocation(ctx->shader, "state"), state);
    glUniform1i(glGetUniformLocation(ctx->shader, "selected"), ctx->selected ? 1 : 0);
  };

  draw_list->AddCallback(callback, context);
  draw_list->AddRectFilled(min, max, 0xFF000000);
  draw_list->PopClipRect();
  draw_list->AddCallback(ImDrawCallback_ResetRenderState, nullptr);

  return ret;
}
