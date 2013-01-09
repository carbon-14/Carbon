#pragma once
#ifndef _GRAPHIC_MATERIALRESOURCE_H
#define _GRAPHIC_MATERIALRESOURCE_H

#include "Graphic/DLL.h"

#include "Core/Resource.h"
#include "Graphic/RenderDevice.h"
#include "Graphic/ProgramCache.h"
#include "Graphic/TextureResource.h"

namespace Graphic
{
    class _GraphicExport MaterialResource : public Core::Resource
    {
    public:
        MaterialResource();
        ~MaterialResource();

        ProgramHandle           GetProgram() const;
        Handle                  GetUniformBuffer() const;
        const TextureResource * GetTexture( SizeT index ) const;
        SizeT                   GetTextureCount() const;

    protected:
        void Load( const void * data );

    private:
        ProgramHandle                       m_program;
        Handle                              m_uniformBuffer;
        Core::SharedPtr< TextureResource >  m_textures[ RenderDevice::ms_maxTextureUnitCount ];
        SizeT                               m_textureCount;
    };
}

#endif // _GRAPHIC_MATERIALRESOURCE_H
