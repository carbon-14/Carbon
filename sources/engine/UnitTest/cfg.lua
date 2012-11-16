
kind                "WindowedApp"
language            "C++"
excludes            { "**__*" }

files               { "*.h", "*.inl", "*.cpp", "cfg.lua" }

d = os.matchdirs( "*" )
for _, code_dir in ipairs( d ) do
    if not string.startswith( code_dir, platform_dir ) then
        files { code_dir.."/**.h", code_dir.."/**.inl", code_dir.."/**.cpp" }
	end
end
for _, ps_dir in ipairs( platform_dirs ) do
    files { ps_dir.."/**.h", ps_dir.."/**.inl", ps_dir.."/**.cpp" }
end

links               { "Core", "Graphic", "OpenGL32" }

if platform_name == "win32" then
    flags { "WinMain" }
end
	
configuration       ( config_debug.name )
    targetdir       ( path.getrelative(prj.basedir,app_dir).."/"..config_debug.name )
    defines         ( config_debug.defines )
    flags           ( config_debug.flags )
    links           { "Core"..config_debug.suffix, "Graphic"..config_debug.suffix }

configuration       ( config_release.name )
    targetdir       ( path.getrelative(prj.basedir,app_dir).."/"..config_release.name )
    defines         ( config_release.defines )
    flags           ( config_release.flags )
    links           { "Core"..config_release.suffix, "Graphic"..config_release.suffix }

configuration       ( config_retail.name )
    targetdir       ( path.getrelative(prj.basedir,app_dir).."/"..config_retail.name )
    defines         ( config_retail.defines )
    flags           ( config_retail.flags )
    links           { "Core"..config_retail.suffix, "Graphic"..config_retail.suffix }
