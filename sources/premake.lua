
-------------------------------------------------------------------------------------------
-- Globals
-------------------------------------------------------------------------------------------

_G.platform_dir = "ps"

if _OPTIONS[ "os" ] == "windows" and _OPTIONS[ "platform" ] == "x32" then
    _G.platform_name    = "win32"
    _G.platform_define  = "CARBON_PLATFORM_WIN32"
    _G.platform_dirs    = { platform_dir.."/win32" }
else
    error( "Invalid platform : ".._OPTIONS[ "os" ].." - ".._OPTIONS[ "platform" ] )
end

_G.bld_dir = "build"
_G.src_dir = "sources"
_G.ext_dir = "ext"
_G.lib_dir = "lib/"..platform_name
_G.app_dir = "bin/"..platform_name

_G.config_debug = {
    name    = "Debug",
    suffix  = "_d",
    kind    = "SharedLib",
    defines = { "DEBUG","CARBON_DEBUG" },
    flags   = { "Symbols" }
}

_G.config_release = {
    name    = "Release",
    suffix  = "_r",
    kind    = "SharedLib",
    defines = { "NDEBUG","CARBON_RELEASE" },
    flags   = { "Optimize" }
}

_G.config_retail = {
    name    = "Retail",
    suffix  = "_s",
    kind    = "StaticLib",
    defines = { "NDEBUG","CARBON_RETAIL" },
    flags   = { "Optimize" }
}

-------------------------------------------------------------------------------------------
-- Prepare solutions
-------------------------------------------------------------------------------------------

s = os.matchdirs( "*" )
for _, sln_basedir in ipairs( s ) do
    sln_name        = path.getname( sln_basedir )
    sln_location    = bld_dir.."/".._ACTION.."/"..sln_name

    print( "[ "..sln_name.." ]" )

    _G.sln          = solution( sln_name )
    _G.sln.basedir  = src_dir.."/"..sln_basedir
    _G.sln.location = sln_location

    includedirs     { "../"..ext_dir.."/inc" }
    for _, ps_dir in ipairs( platform_dirs ) do
        includedirs { "../"..ext_dir.."/inc/"..ps_dir }
    end

    libdirs         { "../"..lib_dir, "../"..ext_dir.."/"..lib_dir }

    dofile( sln_basedir.."/cfg.lua" )
end
