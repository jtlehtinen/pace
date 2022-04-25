workspace "pace"
  location "project/"
  configurations {"debug", "release"}
  language "C++"
  cppdialect "C++20"
  flags { "MultiProcessorCompile", "FatalWarnings" }
  startproject "pace-gui"


filter { "system:windows" }
  platforms { "win64" }


filter { "platforms:win64" }
  defines { "_CRT_SECURE_NO_WARNINGS", "WIN32_LEAN_AND_MEAN", "NOMINMAX" }
  architecture "x64"


include "ext/premake5.lua"


function common_project_configuration()
  systemversion "latest"
  location "project/"
  targetdir "project/bin/%{cfg.platform}-%{cfg.buildcfg}/%{prj.name}"
  editandcontinue "off"

  filter "configurations:debug"
    staticruntime "on"
    runtime "Debug"
    defines { "METRONOME_DEBUG" }
    optimize "off"
    symbols "on"


  filter "configurations:release"
    staticruntime "on"
    runtime "Release"
    optimize "on"
    symbols "on"
end


project "core"
  kind "StaticLib"
  includedirs { "src" }
  files {"src/core/*.h", "src/core/*.cpp"}
  common_project_configuration()

project "pace-cli"
  kind "ConsoleApp"
  includedirs { "src" }
  files {"src/cli/*.h", "src/cli/*.cpp" }
  flags { "NoIncrementalLink", "NoPCH" }
  links { "core" }
  common_project_configuration()


project "pace-gui"
  kind "ConsoleApp"
  includedirs {"src", "ext/imgui", "ext/glfw/include", "ext/glad", "ext/imgui-knobs"}
  files {"src/gui/*.h", "src/gui/*.cpp" }
  flags { "NoIncrementalLink", "NoPCH" }
  links { "core", "glfw", "imgui", "glad", "opengl32.lib" }
  common_project_configuration()

