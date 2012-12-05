#pragma once
#ifndef _MESHCOMPILER_MESHCOMPILER_H
#define _MESHCOMPILER_MESHCOMPILER_H

enum OptionType
{
    OT_COMPRESS_POSITION    = 1,
    OT_COMPRESS_VECTOR      = 1 << 1,
    OT_COMPRESS_TEXCOORD    = 1 << 2,
    OT_COMPRESS_COUNT       = 1 << 3
};

bool CompileMesh( const char * inFilename, const char * outFilename, int options );

#endif
