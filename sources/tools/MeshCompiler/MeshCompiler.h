#pragma once
#ifndef _MESHCOMPILER_MESHCOMPILER_H
#define _MESHCOMPILER_MESHCOMPILER_H

enum CompressionType
{
    CT_POSITION = 1,
    CT_VECTOR   = 1 << 1,
    CT_TEXCOORD = 1 << 2,
    CT_COUNT    = 1 << 3
};

const int CompressionMask = CT_POSITION | CT_VECTOR | CT_TEXCOORD;

enum OptionType
{
    OT_GENERATE_TANGENT_SPACE   = CT_COUNT
};

bool CompileMesh( const char * inFilename, const char * outFilename, int options );

#endif
