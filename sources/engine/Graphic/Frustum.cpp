#include "Graphic/Frustum.h"

namespace Graphic
{
    void Frustum::Update( const Matrix& invViewMatrix, const Vector * viewCorners, const Matrix& viewProjMatrix )
    {
        m_corners[FC_LEFT_BOTTOM_FAR]   = TransformVertex( invViewMatrix, viewCorners[FC_LEFT_BOTTOM_FAR] );
        m_corners[FC_RIGHT_BOTTOM_FAR]  = TransformVertex( invViewMatrix, viewCorners[FC_RIGHT_BOTTOM_FAR] );
        m_corners[FC_LEFT_TOP_FAR]      = TransformVertex( invViewMatrix, viewCorners[FC_LEFT_TOP_FAR] );
        m_corners[FC_RIGHT_TOP_FAR]     = TransformVertex( invViewMatrix, viewCorners[FC_RIGHT_TOP_FAR] );
        m_corners[FC_LEFT_BOTTOM_NEAR]  = TransformVertex( invViewMatrix, viewCorners[FC_LEFT_BOTTOM_NEAR] );
        m_corners[FC_RIGHT_BOTTOM_NEAR] = TransformVertex( invViewMatrix, viewCorners[FC_RIGHT_BOTTOM_NEAR] );
        m_corners[FC_LEFT_TOP_NEAR]     = TransformVertex( invViewMatrix, viewCorners[FC_LEFT_TOP_NEAR] );
        m_corners[FC_RIGHT_TOP_NEAR]    = TransformVertex( invViewMatrix, viewCorners[FC_RIGHT_TOP_NEAR] );

        Matrix tr           = Transpose( viewProjMatrix );
        m_planes[FP_LEFT]   = tr.m_column[3] + tr.m_column[0];
        m_planes[FP_LEFT]   = m_planes[FP_LEFT] / Length( Select( m_planes[FP_LEFT], Zero4(), Mask<0,0,0,1>() ) );
        m_planes[FP_RIGHT]  = tr.m_column[3] - tr.m_column[0];
        m_planes[FP_RIGHT]  = m_planes[FP_RIGHT] / Length( Select( m_planes[FP_RIGHT], Zero4(), Mask<0,0,0,1>() ) );
        m_planes[FP_BOTTOM] = tr.m_column[3] + tr.m_column[1];
        m_planes[FP_BOTTOM] = m_planes[FP_BOTTOM] / Length( Select( m_planes[FP_BOTTOM], Zero4(), Mask<0,0,0,1>() ) );
        m_planes[FP_TOP]    = tr.m_column[3] - tr.m_column[1];
        m_planes[FP_TOP]    = m_planes[FP_TOP] / Length( Select( m_planes[FP_TOP], Zero4(), Mask<0,0,0,1>() ) );
        m_planes[FP_FAR]    = tr.m_column[3] + tr.m_column[2];
        m_planes[FP_FAR]    = m_planes[FP_FAR] / Length( Select( m_planes[FP_FAR], Zero4(), Mask<0,0,0,1>() ) );
        m_planes[FP_NEAR]   = tr.m_column[3] - tr.m_column[2];
        m_planes[FP_NEAR]   = m_planes[FP_NEAR] / Length( Select( m_planes[FP_NEAR], Zero4(), Mask<0,0,0,1>() ) );
    }

    const Vector * Frustum::GetCorners() const
    {
        return m_corners;
    }

    const Vector * Frustum::GetPlanes() const
    {
        return m_planes;
    }
}
