#pragma once
#ifndef _GRAPHIC_FRUSTUM_H
#define _GRAPHIC_FRUSTUM_H

#include "Graphic/DLL.h"

#include "Core/Matrix.h"

namespace Graphic
{
    enum FrustumPlane
    {
        FP_LEFT = 0,
        FP_RIGHT,
        FP_BOTTOM,
        FP_TOP,
        FP_NEAR,
        FP_FAR,
        FP_COUNT
    };

    enum FrustumCorners
    {
        FC_LEFT_BOTTOM_NEAR = 0,
        FC_RIGHT_BOTTOM_NEAR,
        FC_LEFT_TOP_NEAR,
        FC_RIGHT_TOP_NEAR,
        FC_LEFT_BOTTOM_FAR,
        FC_RIGHT_BOTTOM_FAR,
        FC_LEFT_TOP_FAR,
        FC_RIGHT_TOP_FAR,
        FC_COUNT
    };

    class _GraphicExport Frustum
    {
    public:
        void Update( const Matrix& invViewMatrix, const Vector * viewCorners, const Matrix& viewProjMatrix );

        const Vector * GetCorners() const;
        const Vector * GetPlanes() const;

    private:
        Vector  m_corners[FC_COUNT];
        Vector  m_planes[FP_COUNT];
    };
}

#endif // _GRAPHIC_FRUSTUM_H
