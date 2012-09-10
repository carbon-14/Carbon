
kind				"ConsoleApp"
language			"C++"
excludes			{ "**__*" }

files				{ "*.h", "*.inl", "*.cpp", "cfg.lua" }

d = os.matchdirs( "*" )
for _, code_dir in ipairs( d ) do
	if not string.startswith( code_dir, platform_dir ) then
		files	{ code_dir.."/**.h", code_dir.."/**.inl", code_dir.."/**.cpp" }
	end
end
for _, ps_dir in ipairs( platform_dirs ) do
	files	{ ps_dir.."/**.h", ps_dir.."/**.inl", ps_dir.."/**.cpp" }
end

targetdir 			( path.getrelative(prj.basedir,app_dir) )
links				{ "Core" }
	
configuration 		( config_debug.name )
	targetsuffix	( config_debug.suffix )
	defines			( config_debug.defines )
	flags			( config_debug.flags )
	links			{ "Core"..config_debug.suffix }
		
configuration 		( config_release.name )
	targetsuffix	( config_release.suffix )
	defines			( config_release.defines )
	flags			( config_release.flags )
	links			{ "Core"..config_release.suffix }
	