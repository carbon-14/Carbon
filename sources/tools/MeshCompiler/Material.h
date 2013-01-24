#pragma once
#ifndef _MESHCOMPILER_MATERIAL_H
#define _MESHCOMPILER_MATERIAL_H

#include <vector>

const char materialDataDir[] = "data/materials";
const char materialCacheDir[] = "cache/materials";

template< typename T >
bool CmpByLayout( const T& l, const T& r )
{
    return l.layout < r.layout;
}

struct Texture
{
    char            id[32];
    unsigned int    layout;
    char            semantic[32];
    char            default[256];
};

struct Program
{
    char                        id[32];
    std::vector< Texture >      textures;
    std::vector< std::string >  sets;
};

bool FillProgram( Program& prg, const char * root );


#endif // _MESHCOMPILER_MATERIAL_H
