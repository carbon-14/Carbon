
kind				"SharedLib"
language			"C++"
files				{ "**.h", "**.inl", "**.cpp", "cfg.lua" }
excludes			{ "**__*" }

targetdir			( path.getrelative(prj.basedir,lib_dir) )

configuration 		( config_debug.name )
	targetsuffix	( config_debug.suffix )
	defines			( { config_debug.defines, dllexport_define } )
	flags			( config_debug.flags )
		
configuration 		( config_release.name )
	targetsuffix	( config_release.suffix )
	defines			( { config_release.defines, dllexport_define } )
	flags			( config_release.flags )
