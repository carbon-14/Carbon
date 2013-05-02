#include "Graphic/Camera.h"

#include "Core/Math.h"
#include "Core/Matrix.h"

namespace Graphic
{
    void Camera::Update()
    {
        m_invViewMatrix = RMatrix( m_orientation );
        m_invViewMatrix.m_column[3] = m_position;

        m_viewMatrix = Inverse( m_invViewMatrix );

        F32 range = m_near - m_far;

        Vector viewCorners[FC_COUNT];
        if ( m_projectionType == PT_PERSPECTIVE )
        {
            F32 tan     = Tan( 0.5f * m_fov );
            F32 cotan   = 1.0f / tan;

            m_projMatrix.m_column[0] = Vector4( cotan   , 0.0f                  , 0.0f                          , 0.0f  );
            m_projMatrix.m_column[1] = Vector4( 0.0f    , cotan * m_aspectRatio , 0.0f                          , 0.0f  );
            m_projMatrix.m_column[2] = Vector4( 0.0f    , 0.0f                  , ( m_far + m_near ) / range    , -1.0f );
            m_projMatrix.m_column[3] = Vector4( 0.0f    , 0.0f                  , 2.0f * m_near * m_far / range , 0.0f  );

            Vector n = Vector4( m_near, m_near, m_near );
            Vector f = Vector4( m_far, m_far, m_far );
            Vector s = Vector4( tan, tan / m_aspectRatio, 1.0f );

            viewCorners[FC_LEFT_BOTTOM_NEAR]    = n * s * Vector4( -1.0f, -1.0f, -1.0f );
            viewCorners[FC_RIGHT_BOTTOM_NEAR]   = n * s * Vector4( +1.0f, -1.0f, -1.0f );
            viewCorners[FC_LEFT_TOP_NEAR]       = n * s * Vector4( -1.0f, +1.0f, -1.0f );
            viewCorners[FC_RIGHT_TOP_NEAR]      = n * s * Vector4( +1.0f, +1.0f, -1.0f );
            viewCorners[FC_LEFT_BOTTOM_FAR]     = f * s * Vector4( -1.0f, -1.0f, -1.0f );
            viewCorners[FC_RIGHT_BOTTOM_FAR]    = f * s * Vector4( +1.0f, -1.0f, -1.0f );
            viewCorners[FC_LEFT_TOP_FAR]        = f * s * Vector4( -1.0f, +1.0f, -1.0f );
            viewCorners[FC_RIGHT_TOP_FAR]       = f * s * Vector4( +1.0f, +1.0f, -1.0f );
        }
        else
        {
            m_projMatrix.m_column[0] = Vector4( 2.0f / m_width  , 0.0f              , 0.0f                          , 0.0f  );
            m_projMatrix.m_column[1] = Vector4( 0.0f            , 2.0f / m_height   , 0.0f                          , 0.0f  );
            m_projMatrix.m_column[2] = Vector4( 0.0f            , 0.0f              , 2.0f / range                  , 0.0f  );
            m_projMatrix.m_column[3] = Vector4( 0.0f            , 0.0f              , ( m_near + m_far ) / range    , 1.0f  );

            Vector n = Vector4( m_width/2.0f, m_height/2.0f, m_near );
            Vector f = Vector4( m_width/2.0f, m_height/2.0f, m_far );

            viewCorners[FC_LEFT_BOTTOM_NEAR]    = n * Vector4( -1.0f, -1.0f, -1.0f );
            viewCorners[FC_RIGHT_BOTTOM_NEAR]   = n * Vector4( +1.0f, -1.0f, -1.0f );
            viewCorners[FC_LEFT_TOP_NEAR]       = n * Vector4( -1.0f, +1.0f, -1.0f );
            viewCorners[FC_RIGHT_TOP_NEAR]      = n * Vector4( +1.0f, +1.0f, -1.0f );
            viewCorners[FC_LEFT_BOTTOM_FAR]     = f * Vector4( -1.0f, -1.0f, -1.0f );
            viewCorners[FC_RIGHT_BOTTOM_FAR]    = f * Vector4( +1.0f, -1.0f, -1.0f );
            viewCorners[FC_LEFT_TOP_FAR]        = f * Vector4( -1.0f, +1.0f, -1.0f );
            viewCorners[FC_RIGHT_TOP_FAR]       = f * Vector4( +1.0f, +1.0f, -1.0f );
        }

        m_viewScaleNear = viewCorners[FC_RIGHT_TOP_NEAR];
        m_viewScaleFar = viewCorners[FC_RIGHT_TOP_FAR];
        
        m_viewProjMatrix = Mul( m_projMatrix, m_viewMatrix );

        m_frustum.Update( m_invViewMatrix, viewCorners, m_viewProjMatrix );
    }

    const Matrix& Camera::GetViewMatrix() const
    {
        return m_viewMatrix;
    }

    const Matrix& Camera::GetInvViewMatrix() const
    {
        return m_invViewMatrix;
    }

    const Matrix& Camera::GetProjMatrix() const
    {
        return m_projMatrix;
    }

    const Matrix& Camera::GetViewProjMatrix() const
    {
        return m_viewProjMatrix;
    }

    const Vector& Camera::GetViewScaleFar() const
    {
        return m_viewScaleFar;
    }

    const Vector& Camera::GetViewScaleNear() const
    {
        return m_viewScaleNear;
    }

    const Frustum& Camera::GetFrustum() const
    {
        return m_frustum;
    }

    void Camera::ApplyFrustumCulling( const Camera * camera, const Vector * const * inData, SizeT inCount, const Vector ** outData, SizeT& outCount )
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
        Vector zAxis = Select( zPlane, One4, Mask<0,0,0,1>() );
        Vector near = Swizzle<3,3,3,3>( zPlane );
        Vector far = Swizzle<3,3,3,3>( planes[ FP_FAR ] );

        const Vector ** out = outData;

        SizeT r = inCount % 4;
        SizeT q = inCount - r;

        const Vector * const * it = inData;
        const Vector * const * end = inData + q;
        for ( ; it != end; it += 4 )
        {
            const Vector * p0 = it[0];
            const Vector * p1 = it[1];
            const Vector * p2 = it[2];
            const Vector * p3 = it[3];

            const Matrix m = { *p0, *p1, *p2, *p3 };	

            F128 results;
            Store( results, Cull4( m, clip, zPlane, zAxis, near, far ) );

            if ( results[0] ) { *(out++) = p0; }
            if ( results[1] ) { *(out++) = p1; }
            if ( results[2] ) {	*(out++) = p2; }
            if ( results[3] ) { *(out++) = p3; }
        }

        if ( r )
        {
            Matrix m;
            m.m_column[0] = *(end[0]);
            m.m_column[1] = ( r > 1 ) ? *(end[1]) : Zero4;
            m.m_column[2] = ( r > 2 ) ? *(end[2]) : Zero4;
            m.m_column[3] = Zero4;

            F128 results;
            Store( results, Cull4( m, clip, zPlane, zAxis, near, far ) );

            if ( results[0] )			{ *(out++) = end[0]; }
            if ( results[1] && r > 1 )	{ *(out++) = end[1]; }
            if ( results[2] && r > 2 )	{ *(out++) = end[2]; }
        }

        outCount = out - outData;
    }

    Vector Camera::Cull4( const Matrix& m, const Matrix& clip, const Vector& zPlane, const Vector& zAxis, const Vector& near, const Vector& far )
    {
        Vector c0 = Select( m.m_column[0], One4, Mask<0,0,0,1>() );
        Vector c1 = Select( m.m_column[1], One4, Mask<0,0,0,1>() );
        Vector c2 = Select( m.m_column[2], One4, Mask<0,0,0,1>() );
        Vector c3 = Select( m.m_column[3], One4, Mask<0,0,0,1>() );
        Vector r = Shuffle<0,2,0,2>( Shuffle<3,3,3,3>( m.m_column[0], m.m_column[1] ), Shuffle<3,3,3,3>( m.m_column[2], m.m_column[3] ) );

        Matrix c = { c0, c1, c2, c3 };
        c = Transpose( c );

        Matrix clipProj	= Mul( c, clip );
        Vector zProj	= Mul( c, zAxis );

        Vector test;
        test = GreaterThan( clipProj.m_column[0], r );
        test = Or( test, GreaterThan( clipProj.m_column[1], r ) );
        test = Or( test, GreaterThan( clipProj.m_column[2], r ) );
        test = Or( test, GreaterThan( clipProj.m_column[3], r ) );

        test = Or( test, GreaterThan( zProj + near, r ) );
        test = Or( test, LessThan( zProj + far, r ) );
        test = AndNot( test, Splat( F32( 0xffffffff ) ) );

        return test;
    }
}
