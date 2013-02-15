#pragma once
#ifndef _GRAPHIC_MATERIALRESOURCE_H
#define _GRAPHIC_MATERIALRESOURCE_H

#include "Graphic/DLL.h"

#include "Core/Resource.h"
#include "Graphic/ProgramCache.h"
#include "Graphic/TextureResource.h"

namespace Graphic
{
    class _GraphicExport MaterialResource : public Resource
    {
    public:
        struct Texture
        {
            SharedPtr< TextureResource >    m_resource;
            SizeT                           m_index;
        };

    public:
        MaterialResource();
        ~MaterialResource();

        ProgramHandle   GetProgram() const;
        const Texture&  GetTexture( SizeT index ) const;
        SizeT           GetTextureCount() const;

    protected:
        Bool Load( const void * data );
        void Unload();

    private:
        ProgramHandle   m_program;
        Texture         m_textures[ s_maxTextureUnitCount ];
        SizeT           m_textureCount;
    };
}

#endif // _GRAPHIC_MATERIALRESOURCE_H
