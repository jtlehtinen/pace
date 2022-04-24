filter { "system:windows" }
  platforms { "win64" }


filter { "platforms:win64" }
  defines { "_CRT_SECURE_NO_WARNINGS", "WIN32_LEAN_AND_MEAN", "NOMINMAX" }
  architecture "x64"


project "glfw"
  kind "StaticLib"
  language "C"
  staticruntime "On"
  targetdir "../project/bin/%{cfg.platform}-%{cfg.buildcfg}/%{prj.name}"

  files {
    "glfw/include/GLFW/glfw3.h",
    "glfw/include/GLFW/glfw3native.h",
    "glfw/src/glfw_config.h",
    "glfw/src/context.c",
    "glfw/src/init.c",
    "glfw/src/input.c",
    "glfw/src/monitor.c",
    "glfw/src/null_init.c",
    "glfw/src/null_joystick.c",
    "glfw/src/null_monitor.c",
    "glfw/src/null_window.c",
    "glfw/src/platform.c",
    "glfw/src/vulkan.c",
    "glfw/src/window.c",
  }

  filter "system:linux"
    systemversion "latest"
    files {
      "glfw/src/x11_init.c",
      "glfw/src/x11_monitor.c",
      "glfw/src/x11_window.c",
      "glfw/src/xkb_unicode.c",
      "glfw/src/posix_time.c",
      "glfw/src/posix_thread.c",
      "glfw/src/glx_context.c",
      "glfw/src/egl_context.c",
      "glfw/src/osmesa_context.c",
      "glfw/src/linux_joystick.c"
    }

    defines {
      "_GLFW_X11"
    }

  filter "system:windows"
    systemversion "latest"

    files {
      "glfw/src/egl_context.c",
      "glfw/src/osmesa_context.c",
      "glfw/src/wgl_context.c",
      "glfw/src/win32_init.c",
      "glfw/src/win32_joystick.c",
      "glfw/src/win32_module.c",
      "glfw/src/win32_monitor.c",
      "glfw/src/win32_time.c",
      "glfw/src/win32_thread.c",
      "glfw/src/win32_window.c",
    }

    defines {
      "_GLFW_WIN32",
      "_CRT_SECURE_NO_WARNINGS"
    }

    links {
      "Dwmapi.lib"
    }

  filter "configurations:debug"
    runtime "Debug"
    symbols "On"

  filter "configurations:release"
    runtime "Release"
    optimize "On"
    symbols "On"


project "imgui"
  kind "StaticLib"
  language "C++"
  staticruntime "On"
  targetdir "../project/bin/%{cfg.platform}-%{cfg.buildcfg}/%{prj.name}"

  files {
    "imgui/imconfig.h",
    "imgui/imgui.h",
    "imgui/imgui.cpp",
    "imgui/imgui_draw.cpp",
    "imgui/imgui_internal.h",
    "imgui/imgui_tables.cpp",
    "imgui/imgui_widgets.cpp",
    "imgui/imstb_rectpack.h",
    "imgui/imstb_textedit.h",
    "imgui/imstb_truetype.h",
    "imgui/imgui_demo.cpp"
  }

  filter "system:windows"
    systemversion "latest"
    cppdialect "C++20"

  filter "system:linux"
    systemversion "latest"
    cppdialect "C++20"

  filter "configurations:debug"
    runtime "Debug"
    symbols "On"

  filter "configurations:release"
    runtime "Release"
    optimize "On"
    symbols "On"
