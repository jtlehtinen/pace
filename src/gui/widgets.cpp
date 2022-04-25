#include "widgets.h"
#include "ogl.h"
#include <string>

bool Widgets::Tempo(uint32_t* shader, float width, float height) {
  // @TODO: Mess, clean it...
  // @TODO: Interaction...

  ImVec2 size(width, height);
  bool ret = ImGui::InvisibleButton("tempo-foobar", size);

  ImVec2 min = ImGui::GetItemRectMin();
  ImVec2 max = ImGui::GetItemRectMax();

  ImDrawList* draw_list = ImGui::GetWindowDrawList();
  draw_list->PushClipRect(min, max);

  auto callback = [](const ImDrawList* draw_list, const ImDrawCmd* draw_cmd) {
    uint32_t* shader = (uint32_t*)draw_cmd->UserCallbackData;

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

    glUseProgram(*shader); // If I remove this line, it works
    glUniformMatrix4fv(glGetUniformLocation(*shader, "ProjMtx"), 1, GL_FALSE, &ortho_projection[0][0]);
    glUniform1f(glGetUniformLocation(*shader, "Time"), time);

    float minx = clip_min.x;
    float miny = draw_data->DisplaySize.y - clip_max.y;
    float maxx = clip_max.x;
    float maxy = draw_data->DisplaySize.y - clip_min.y;

    glUniform4f(glGetUniformLocation(*shader, "BBox"), minx, miny, maxx, maxy);
  };

  draw_list->AddCallback(callback, shader);
  draw_list->AddRectFilled(min, max, 0xFF000000);
  draw_list->PopClipRect();
  draw_list->AddCallback(ImDrawCallback_ResetRenderState, nullptr);

  return ret;
}
