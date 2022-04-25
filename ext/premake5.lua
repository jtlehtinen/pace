project "glfw"
  kind "StaticLib"
  editandcontinue "off"
  language "C"
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

  filter "configurations:debug"
    staticruntime "on"
    runtime "Debug"
    symbols "on"

  filter "configurations:release"
    staticruntime "on"
    runtime "Release"
    optimize "on"
    symbols "on"


project "imgui"
  kind "StaticLib"
  editandcontinue "off"
  language "C++"
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
    staticruntime "on"
    runtime "Debug"
    symbols "on"

  filter "configurations:release"
    staticruntime "on"
    runtime "Release"
    optimize "on"
    symbols "on"


project "glad"
  kind "StaticLib"
  editandcontinue "off"
  language "C"
  targetdir "../project/bin/%{cfg.platform}-%{cfg.buildcfg}/%{prj.name}"

  files {
    "glad/glad.h",
    "glad/glad.c",
    "glad/KHR/khrplatform.h",
  }

  filter "system:windows"
    systemversion "latest"

  filter "system:linux"
    systemversion "latest"

  filter "configurations:debug"
    staticruntime "on"
    runtime "Debug"
    symbols "on"

  filter "configurations:release"
    staticruntime "on"
    runtime "Release"
    optimize "on"
    symbols "on"
