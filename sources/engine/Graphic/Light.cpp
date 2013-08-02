#include "Graphic/Light.h"

#include "Core/Math.h"
#include "Core/Matrix.h"

namespace Graphic
{
    void Light::Update()
    {
        switch ( m_type )
        {
        case LT_DIRECTIONAL:
            {
                Vector diag = Vector4( m_directionalWidth, m_directionalHeight, m_radius, 0.0 );

                Vector rad = Splat( 0.5f ) * Sqrt( Dot( diag, diag ) );

                Matrix rot = RMatrix( m_orientation );
                Vector pos = m_position - Splat( 0.5f ) * rot.m_column[2];
                
                m_boundingSphere = Select( pos, rad, Mask<0,0,0,1>() );

                break;
            }
        case LT_OMNI:
            {
                m_boundingSphere = Select( m_position, Splat( m_radius ), Mask<0,0,0,1>() );
                break;
            }
        case LT_SPOT:
            {
                if ( m_spotOutAngle < TwoPi / 3.0f )
                {
                    F32 rad = m_radius / ( 2.0f * Cos( 0.5f * m_spotOutAngle ) );

                    Matrix rot = RMatrix( m_orientation );
                    Vector pos = m_position - Splat( rad ) * rot.m_column[2];
                
                    m_boundingSphere = Select( pos, Splat(rad), Mask<0,0,0,1>() );
                }
                else
                {
                    m_boundingSphere = Select( m_position, Splat( m_radius ), Mask<0,0,0,1>() );
                }
                Vector diag = Vector4( m_directionalWidth, m_directionalHeight, m_radius, 0.0 );

                Vector rad = Splat( 0.5f ) * Sqrt( Dot( diag, diag ) );

                
                break;
            }
        }
    }

    Vector Light::GetBoundingSphere() const
    {
        return m_boundingSphere;
    }
}
