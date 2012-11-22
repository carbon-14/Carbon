#pragma once
#ifndef _TEXTURECOMPILER_TEXTURECOMPILER_H
#define _TEXTURECOMPILER_TEXTURECOMPILER_H

enum CompressionFormat
{
    CF_BC1,
    CF_BC2,
    CF_BC3,
    CF_BC4,
    CF_BC5,
    CF_BC6,
    CF_BC7
};

bool CompileTexture( const char * inFilename, const char * outFilename, CompressionFormat compression );

#endif // _TEXTURECOMPILER_TEXTURECOMPILER_H
