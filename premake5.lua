workspace "backface-cull"
	configurations { "ReleaseIII", "DebugIII" }
	location "project_files"
   
project "backface-cull"
	kind "SharedLib"
	language "C++"
	targetdir "output/asi/"
	objdir ("output/obj")
	targetextension ".asi"
	characterset ("MBCS")
	linkoptions "/SAFESEH:NO"
	buildoptions { "-std:c++latest", "/permissive" }
	defines { "_CRT_SECURE_NO_WARNINGS", "_CRT_NON_CONFORMING_SWPRINTFS", "_USE_MATH_DEFINES" }
	disablewarnings { "4244", "4800", "4305", "4073", "4838", "4996", "4221", "4430", "26812", "26495", "6031" }

	files {
		"source/**.*",
	}
	
	includedirs { 
		"source/**",
		"vendor/**"
	}
	
	includedirs {
		"vendor/GInputAPI",
		"$(PLUGIN_SDK_DIR)/shared/",
		"$(PLUGIN_SDK_DIR)/shared/game/",
	}
	
	filter { "configurations:*III" }
		defines { "GTA3", "PLUGIN_SGV_10EN" }
		includedirs {
			"$(PLUGIN_SDK_DIR)/plugin_III/",
			"$(PLUGIN_SDK_DIR)/plugin_III/game_III/",
		}
		targetname "BackfaceCullingIII"
		debugdir "$(GTA_III_DIR)"
		debugcommand "$(GTA_III_DIR)/gta3.exe"
		postbuildcommands "copy /y \"$(TargetPath)\" \"$(GTA_III_DIR)\\scripts\\BackfaceCullingIII.asi\""
	
	filter { }
	
	libdirs { 
		"$(PLUGIN_SDK_DIR)/output/lib/",
		"$(DXSDK_DIR)/Lib/x86",
	}
	
	filter "configurations:Debug*"		
		defines { "DEBUG" }
		symbols "on"
		staticruntime "on"

	filter "configurations:Release*"
		defines { "NDEBUG" }
		symbols "off"
		optimize "On"
		staticruntime "on"
		
	filter "configurations:ReleaseIII"
		links { "plugin_iii" }

	filter "configurations:DebugIII"
		links { "plugin_iii_d" }
			
	filter { }
