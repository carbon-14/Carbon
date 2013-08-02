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

        template < typename T >
        static void ApplyFrustumCulling( const Camera * camera, const T * const * inData, SizeT inCount, const T ** outData, SizeT& outCount );

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

    template < typename T >
    void Camera::ApplyFrustumCulling( const Camera * camera, const T * const * inData, SizeT inCount, const T ** outData, SizeT& outCount )
    {
        const Frustum& frustum = camera->GetFrustum();
        const Vector * planes = frustum.GetPlanes();

        Matrix clip =
        {
            -planes[ FP_LEFT ],
            -planes[ FP_RIGHT ],
            -planes[ FP_BOTTOM ],
            -planes[ FP_TOP ]
        };

        Vector zPlane = -planes[ FP_NEAR ];
        Vector zAxis = Select( zPlane, Zero4, Mask<0,0,0,1>() );
        Vector near = Swizzle<3,3,3,3>( zPlane );
        Vector far = Swizzle<3,3,3,3>( planes[ FP_FAR ] );

        const T ** out = outData;

        SizeT r = inCount % 4;
        SizeT q = inCount - r;

        const T * const * it = inData;
        const T * const * end = inData + q;
        for ( ; it != end; it += 4 )
        {
            const Matrix m =
            {
                it[0]->GetBoundingSphere(),
                it[1]->GetBoundingSphere(),
                it[2]->GetBoundingSphere(),
                it[3]->GetBoundingSphere()
            };

            F128 results;
            Store( results, Cull4( m, clip, zPlane, zAxis, near, far ) );

            if ( results[0] ) { *(out++) = it[0]; }
            if ( results[1] ) { *(out++) = it[1]; }
            if ( results[2] ) {	*(out++) = it[2]; }
            if ( results[3] ) { *(out++) = it[3]; }
        }

        if ( r )
        {
            Matrix m;
            m.m_column[0] = end[0]->GetBoundingSphere();
            m.m_column[1] = ( r > 1 ) ? end[1]->GetBoundingSphere() : Zero4;
            m.m_column[2] = ( r > 2 ) ? end[2]->GetBoundingSphere() : Zero4;
            m.m_column[3] = Zero4;

            F128 results;
            Store( results, Cull4( m, clip, zPlane, zAxis, near, far ) );

            if ( results[0] )			{ *(out++) = end[0]; }
            if ( results[1] && r > 1 )	{ *(out++) = end[1]; }
            if ( results[2] && r > 2 )	{ *(out++) = end[2]; }
        }

        outCount = out - outData;
    }
}

#endif // _GRAPHIC_CAMERA_H
