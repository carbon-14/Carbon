
-------------------------------------------------------------------------------------------
-- Prepare tools Projects
-------------------------------------------------------------------------------------------

configurations  { "Debug", "Release" }
defines         { platform_define }

p = os.matchdirs( "*" )
for _, prj_basedir in ipairs( p ) do
    prj_name        = path.getname( prj_basedir )
    prj_location    = sln.location.."/"..prj_name

    print( "[ -> "..prj_name.." ]" )

    _G.prj          = project( prj_name )
    _G.prj.basedir  = sln.basedir.."/"..prj_basedir
    _G.prj.location = prj_location
    _G.prj.objdir   = prj_location.."/obj/"..platform_name

    vpaths          { [""] = prj.basedir }
    includedirs     { "." }

    dofile( prj_basedir.."/cfg.lua" )
end
