
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
_G.lib_dir = "lib/"..platform_name
_G.app_dir = "bin/"..platform_name

_G.config_debug = {
    name    = "Debug",
    suffix  = "_d",
    defines = { "DEBUG" },
    flags   = { "Symbols" }
}

_G.config_release = {
    name    = "Release",
    suffix  = "_r",
    defines = { "NDEBUG" },
    flags   = { "Optimize" }
}

_G.dllexport_define	= "CARBON_CORE_DLLEXPORT"

-------------------------------------------------------------------------------------------
-- Prepare solution 'engine'
-------------------------------------------------------------------------------------------

sln_name        = "engine"
sln_basedir     = src_dir.."/"..sln_name
sln_location    = bld_dir.."/".._ACTION.."/"..sln_name

_G.sln          = solution( sln_name )
_G.sln.basedir  = sln_basedir
_G.sln.location = sln_location

-------------------------------------------------------------------------------------------
-- Prepare Projects in solution directory
-------------------------------------------------------------------------------------------

configurations  { "Debug", "Release" }
defines         { platform_define }

p = os.matchdirs( sln_basedir.."/*" )
for _, prj_basedir in ipairs( p ) do
    prj_name        = path.getname( prj_basedir )
    prj_location    = sln_location.."/"..prj_name

    print( "[ "..sln_name..":"..prj_name.." ]" )

    _G.prj          = project( prj_name )
    _G.prj.basedir  = prj_basedir
    _G.prj.location = prj_location
    _G.prj.objdir   = prj_location.."/obj/"..platform_name
	
    vpaths          { [""] = prj_basedir }
    includedirs     { prj_basedir, sln_basedir }
    libdirs         { lib_dir }

    dofile( prj_basedir.."/cfg.lua" )
end
