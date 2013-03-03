#pragma once
#ifndef _GRAPHIC_LIGHTGEOMETRY_H
#define _GRAPHIC_LIGHTGEOMETRY_H

#include "Graphic/Types.h"
#include "Graphic/RenderGeometry.h"

namespace Graphic
{
    class DirectionalGeometry : public RenderGeometry
    {
    public:
        void Initialize();
        void Destroy();

        void Draw() const;

    private:
        Handle  m_vertexBuffer;
        Handle  m_indexBuffer;
        Handle  m_vertexArray;
    };

    class OmniGeometry : public RenderGeometry
    {
    public:
        void Initialize();
        void Destroy();

        void Draw() const;

    private:
        Handle  m_vertexBuffer;
        Handle  m_indexBuffer;
        Handle  m_vertexArray;
    };

    class SpotGeometry : public RenderGeometry
    {
    public:
        void Initialize();
        void Destroy();

        void Draw() const;

    private:
        Handle  m_vertexBuffer;
        Handle  m_indexBuffer;
        Handle  m_vertexArray;
    };
};

#endif // _GRAPHIC_LIGHTGEOMETRY_H
