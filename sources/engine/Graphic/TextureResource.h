#pragma once
#ifndef _GRAPHIC_TEXTURERESOURCE_H
#define _GRAPHIC_TEXTURERESOURCE_H

#include "Graphic/DLL.h"

#include "Core/Resource.h"
#include "Graphic/RenderDevice.h"

namespace Graphic
{
    class _GraphicExport TextureResource : public Core::Resource
    {
    public:
        TextureResource();
        ~TextureResource();

        Handle GetTexture() const;

    protected:
        bool Load( const void * data );
        void Unload();

    private:
        Handle  m_texture;
    };
}

#endif // _GRAPHIC_TEXTURERESOURCE_H
