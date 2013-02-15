#pragma once
#ifndef _GRAPHIC_TEXTURERESOURCE_H
#define _GRAPHIC_TEXTURERESOURCE_H

#include "Graphic/DLL.h"

#include "Core/Resource.h"
#include "Graphic/Types.h"

namespace Graphic
{
    class _GraphicExport TextureResource : public Resource
    {
    public:
        TextureResource();
        ~TextureResource();

        Handle GetTexture() const;

    protected:
        Bool Load( const void * data );
        void Unload();

    private:
        Handle  m_texture;
    };
}

#endif // _GRAPHIC_TEXTURERESOURCE_H
