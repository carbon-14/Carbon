
kind				"ConsoleApp"
language			"C++"
files				{ "**.h", "**.inl", "**.cpp", "cfg.lua" }
excludes			{ "**__*" }

targetdir 			( path.getrelative(prj.basedir,app_dir) )
links				{ "Core" }
	
configuration 		( config_debug.name )
	targetsuffix	( config_debug.suffix )
	defines			( config_debug.defines )
	flags			( config_debug.flags )
		
configuration 		( config_release.name )
	targetsuffix	( config_release.suffix )
	defines			( config_release.defines )
	flags			( config_release.flags )
	