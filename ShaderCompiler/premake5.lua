MacOSVersion = MacOSVersion or "14.5"

project "ShaderCompiler"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "On"

	architecture "x86_64"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.hpp",
		"src/**.inl",
		"src/**.cpp",
		"src/**.cc",

		"include/glslang/**.c",
		"include/glslang/**.h",
		"include/glslang/**.hpp",
		"include/glslang/**.cpp",
		"include/glslang/**.cc",

		"include/glslangSPIRV/**.c",
		"include/glslangSPIRV/**.h",
		"include/glslangSPIRV/**.hpp",
		"include/glslangSPIRV/**.cpp",
		"include/glslangSPIRV/**.cc",
	}

	removefiles
	{
		"include/glslang/OSDependent/**"
	}

	defines
	{
		"ENABLE_HLSL",

		"_CRT_SECURE_NO_WARNINGS"
	}

	includedirs
	{
		"src",
		"include",
	}

	filter "system:windows"
		defines "SPIRV_WINDOWS"
		systemversion "latest"
		staticruntime "on"
		
		files
		{
			"include/glslang/OSDependent/Windows/**.c",
			"include/glslang/OSDependent/Windows/**.h",
			"include/glslang/OSDependent/Windows/**.hpp",
			"include/glslang/OSDependent/Windows/**.cpp",
			"include/glslang/OSDependent/Windows/**.cc",
		}
		
	filter "system:linux"
		defines "SPIRV_UNIX" -- TODO: Check if works
		systemversion "latest"
		staticruntime "on"
		
		files
		{
			"include/glslang/OSDependent/Unix/**.c",
			"include/glslang/OSDependent/Unix/**.h",
			"include/glslang/OSDependent/Unix/**.hpp",
			"include/glslang/OSDependent/Unix/**.cpp",
			"include/glslang/OSDependent/Unix/**.cc",
		}
		
	filter "system:macosx"
		defines "SPIRV_UNIX" -- TODO: Check if works
		systemversion(MacOSVersion)
		staticruntime "on"

		files
		{
			"include/glslang/OSDependent/Unix/**.c",
			"include/glslang/OSDependent/Unix/**.h",
			"include/glslang/OSDependent/Unix/**.hpp",
			"include/glslang/OSDependent/Unix/**.cpp",
			"include/glslang/OSDependent/Unix/**.cc",
		}

	filter "action:xcode*"
		-- Note: If we don't add the header files to the externalincludedirs
		-- we can't use <angled> brackets to include files.
		externalincludedirs
		{
			"src",
			"include",
		}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		runtime "Release"
		optimize "Full"
