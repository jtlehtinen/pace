#include <Windows.h>
#include <stdio.h>
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui.h"
#include <GLFW/glfw3.h>
#include "application.h"

namespace OpenSansFont {
#include "opensans.embed"
}

static void glfw_error_callback(int error, const char* description) {
  fprintf(stderr, "glfw Error %d: %s\n", error, description);
}

int __stdcall wWinMain(HINSTANCE instance, HINSTANCE previousInstance, LPWSTR cmdLine, int showCode) {
  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit())
    return 1;

  const char* glsl_version = "#version 330 core";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  GLFWwindow* window = glfwCreateWindow(400, 570, "pace", nullptr, nullptr);
  if (window == nullptr)
    return 1;

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  //io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

  ImGui::StyleColorsDark();

  ImGuiStyle& style = ImGui::GetStyle();
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    style.WindowRounding = 0.0f;
    style.Colors[ImGuiCol_WindowBg].w = 1.0f;
  }

  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  ImFontConfig font_config;
  font_config.FontDataOwnedByAtlas = false;
  ImFont* open_sans_font = io.Fonts->AddFontFromMemoryTTF((void*)OpenSansFont::data, sizeof(OpenSansFont::data), 42.0f, &font_config);
  ImFont* open_sans_font_large = io.Fonts->AddFontFromMemoryTTF((void*)OpenSansFont::data, sizeof(OpenSansFont::data), 128.0f, &font_config);
  io.FontDefault = open_sans_font;

  ImGuiFonts fonts = {
    .normal = open_sans_font,
    .large = open_sans_font_large,
  };

  Application application;
  if (application.Initialize(fonts)) {
    while (!glfwWindowShouldClose(window)) {
      glfwPollEvents();

      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();

      int vw, vh;
      glfwGetFramebufferSize(window, &vw, &vh);
      glViewport(0, 0, vw, vh);
      glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT);

      application.Render(vw, vh);
      ImGui::Render();

      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

      if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
      }

      glfwSwapBuffers(window);
    }

    application.Terminate();
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}
