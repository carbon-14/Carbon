#pragma once
#ifndef _GRAPHIC_CAMERA_H
#define _GRAPHIC_CAMERA_H

#include "Graphic/DLL.h"
#include "Graphic/Frustum.h"

#include "Core/Vector.h"
#include "Core/Matrix.h"

namespace Graphic
{
    enum ProjectionType
    {
        PT_ORTHOGRAPHIC,
        PT_PERSPECTIVE
    };

    class _GraphicExport Camera
    {
    public:
        void Update();

        const Matrix& GetViewMatrix() const;
        const Matrix& GetInvViewMatrix() const;
        const Matrix& GetProjMatrix() const;
        const Matrix& GetViewProjMatrix() const;
        const Vector& GetViewScaleFar() const;
        const Vector& GetViewScaleNear() const;
        const Frustum& GetFrustum() const;

        static void ApplyFrustumCulling( const Camera * camera, const Vector * const * inData, SizeT inCount, const Vector ** outData, SizeT& outCount );

    public:
        Vector          m_position;
        Vector          m_orientation;
        F32             m_width;
        F32             m_height;
        F32             m_fov;
        F32             m_aspectRatio;
        F32             m_near;
        F32             m_far;
        ProjectionType  m_projectionType;

    private:
        static Vector Cull4( const Matrix& m, const Matrix& clip, const Vector& zPlane, const Vector& zAxis, const Vector& near, const Vector& far );

        Matrix          m_viewMatrix;
        Matrix          m_invViewMatrix;
        Matrix          m_projMatrix;
        Matrix          m_viewProjMatrix;
        Vector          m_viewScaleNear;
        Vector          m_viewScaleFar;
        Frustum         m_frustum;
    };
}

#endif // _GRAPHIC_CAMERA_H
