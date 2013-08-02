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

    Vector Camera::Cull4( const Matrix& m, const Matrix& clip, const Vector& zPlane, const Vector& zAxis, const Vector& near, const Vector& far )
    {
        Matrix c = Transpose( m );
        Vector r = c.m_column[3];
        c.m_column[3] = One4;

        Matrix clipProj	= Mul( c , clip );
        Vector zProj	= Mul( c, zAxis );

        Vector test;
        test = clipProj.m_column[0] > r;
        test = Or( test, clipProj.m_column[1] > r );
        test = Or( test, clipProj.m_column[2] > r );
        test = Or( test, clipProj.m_column[3] > r );

        test = Or( test, ( zProj + near ) > r );
        test = Or( test, ( zProj + far ) < r );
        test = AndNot( test, Splat( F32( 0xffffffff ) ) );

        return test;
    }
}
