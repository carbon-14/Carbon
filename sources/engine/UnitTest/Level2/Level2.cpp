#include "UnitTest/Level2/Level2.h"
#include "UnitTest/Utils.h"

#include "Core/Math.h"
#include "Core/Vector.h"
#include "Core/Matrix.h"

using namespace Core;

void Level2()
{
    UNIT_TEST_MESSAGE( "\n###########\n# LEVEL 2 #\n###########\n\n" )

    Vector v0 = Vector4( 1.0f, 2.0f, 3.0f, 4.0f );
    Vector v1 = Vector3( 1.0f, 2.0f, 3.0f );
    Vector v2 = Vector2( 1.0f, 2.0f );
    Vector v3 = Splat( Math::Pi() );

    F128 a;
    Store( a, v1 );
    a[ 1 ] = 42.0f;
    v1 = Load( a );

    Vector v4 = Swizzle< 3, 2, 1, 0 >( v0 );

    Vector v5 = Dot( v1, Swizzle< 2, 1, 0, 3 >( v1 ) );
    Vector v6 = Cross( UnitX(), UnitY() );

    // ======================================================

    Vector cam_pos  = Vector4( 1.0f, 2.0f, 3.0f );
    Vector cam_at   = Vector4( 6.0f, 5.0f, 4.0f );
    Vector cam_dist = Length( cam_at - cam_pos );

    Vector cam_dir      = Normalize( cam_at - cam_pos );
    Vector cam_up       = UnitY();
    Vector cam_right    = Normalize( Cross( cam_dir, cam_up ) );
    cam_up              = Normalize( Cross( cam_right, cam_dir ) );

    Matrix cam_base;
    cam_base.m_column[0] = cam_right;
    cam_base.m_column[1] = cam_up;
    cam_base.m_column[2] = -cam_dir;
    cam_base.m_column[3] = cam_pos;

    Matrix view = Inverse( cam_base );

    Vector cam_test = TransformVertex( view, cam_at );

    F128 out_cam_pos, out_cam_at, out_cam_test, out_cam_dist;
    Store( out_cam_pos, cam_pos );
    Store( out_cam_at, cam_at );
    Store( out_cam_test, cam_test );
    Store( out_cam_dist, cam_dist );

    UNIT_TEST_MESSAGE( "Position Camera : ( %0.2f, %0.2f, %0.2f )\n", out_cam_pos[0], out_cam_pos[1], out_cam_pos[2] );
    UNIT_TEST_MESSAGE( "Position Look At : ( %0.2f, %0.2f, %0.2f )\n", out_cam_at[0], out_cam_at[1], out_cam_at[2] )
    UNIT_TEST_MESSAGE( "Position Look At dans le repere de la camera : ( %0.2f, %0.2f, %0.2f )\n", out_cam_test[0], out_cam_test[1], out_cam_test[2] )
    UNIT_TEST_MESSAGE( "Distance Camera - Look At : %0.2f\n", out_cam_dist[0] )

}
