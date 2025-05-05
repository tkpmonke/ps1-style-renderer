workspace "retro"
   architecture "x86_64"
   configurations { "Debug", "Release"}

project "retro"
   kind "ConsoleApp"
   language "C"
   cdialect "c99"
   targetdir "bin"
   includedirs { "include", "external" }
   files { "src/**.c", "include/**.h" }
   
   links { "GL", "glfw", "GLEW", "cglm", "m" }

	warnings "High"

   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"
