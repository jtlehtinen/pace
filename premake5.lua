workspace "pace"
  location "project/"
  configurations {"debug", "release"}
  language "C++"
  cppdialect "C++20"
  flags { "MultiProcessorCompile", "FatalWarnings" }

include "ext/premake5.lua"


filter { "system:windows" }
  platforms { "win64" }


filter { "platforms:win64" }
  defines { "_CRT_SECURE_NO_WARNINGS", "WIN32_LEAN_AND_MEAN", "NOMINMAX" }
  architecture "x64"


filter "configurations:debug"
  runtime "Debug"
  defines { "METRONOME_DEBUG" }
  optimize "Off"
  symbols "On"


filter "configurations:release"
  runtime "Release"
  optimize "On"
  symbols "On"


function common_project_configuration()
  systemversion "latest"
  location "project/"
  targetdir "project/bin/%{cfg.platform}-%{cfg.buildcfg}/%{prj.name}"
  includedirs { "src" }
  editandcontinue "Off"
  staticruntime "On"
end


project "core"
  common_project_configuration()
  kind "StaticLib"
  files {"src/core/*.h", "src/core/*.cpp"}


project "pace-cli"
  common_project_configuration()
  kind "ConsoleApp"
  files {"src/cli/*.h", "src/cli/*.cpp" }
  flags { "NoIncrementalLink", "NoPCH" }
  links { "core" }


project "pace-gui"
  common_project_configuration()
  kind "WindowedApp"
  files {"src/gui/*.h", "src/gui/*.cpp" }
  flags { "NoIncrementalLink", "NoPCH" }
  links { "core", "glfw", "imgui" }
