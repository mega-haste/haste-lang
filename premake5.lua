workspace "haste-lang"
  configurations { "Debug", "Release" }
  location "build"

project "haste"
  targetname "haste"
  kind "ConsoleApp"
  language "C++"
  targetdir "./build/bin/%{cfg.buildcfg}"

  libdirs { "termcolor" }
  includedirs { "include" }
  files { "src/**.cpp", "main.cpp" }

  cppdialect "C++20"
  buildoptions { "-std=c++20" }

  filter "configurations:Debug"
    warnings "Extra"
    defines { "DEBUG" }
    symbols "On"

  filter "configurations:Release"
    defines { "NDEBUG" }
    optimize "On"
