workspace "pace"
  location "project/"
  configurations {"debug", "release"}
  language "C++"
  cppdialect "C++20"
  flags { "MultiProcessorCompile", "FatalWarnings" }

filter { "system:windows" }
  platforms { "win64" }

filter { "platforms:Win64" }
defines { "WIN32", "_CRT_SECURE_NO_WARNINGS", "WIN32_LEAN_AND_MEAN", "NOMINMAX" }
architecture "x64"

filter "configurations:debug"
  defines { "METRONOME_DEBUG" }
  symbols "On"

filter "configurations:release"
  optimize "On"

project "pace"
  location "project/"
  kind "ConsoleApp"
  targetdir "project/bin/%{cfg.platform}/%{cfg.buildcfg}"
  files {"src/**.h", "src/**.cpp"}
  includedirs { "src" }
  editandcontinue "Off"
  staticruntime "On"
  flags { "NoIncrementalLink", "NoPCH" }
