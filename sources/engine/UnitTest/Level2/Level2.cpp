#include "UnitTest/Level2/Level2.h"
#include "UnitTest/Utils.h"

#include "Core/Math.h"
#include "Core/Vector.h"
#include "Core/Matrix.h"
#include "Core/Quaternion.h"

namespace Level2_NS
{

    Char * SerializeVector( Char * dest, const Vector& v )
    {
        F128 s;
        Store( s, v );

        return StringUtils::FormatString( dest, 32, "%0.2f %0.2f %0.2f %0.2f", s[0], s[1], s[2], s[3] );
    }
}

using namespace Level2_NS;

void Level2()
{
    UNIT_TEST_MESSAGE( "\n###########\n# LEVEL 2 #\n###########\n\n" );

    Char v_serial[32];
    Matrix tmp;

    // Creation du triangle
    // ( triangle equilateral unitaire dans le plan Z )

    Vector triangle[ 3 ];
    triangle[ 0 ] = Vector4( 1.0f, 0.0f, 0.0f );
    triangle[ 1 ] = Vector4( -0.5f, 0.5f * Sqrt( 3.0f ), 0.0f );
    triangle[ 2 ] = Vector4( -0.5f, -0.5f * Sqrt( 3.0f ), 0.0f );

    UNIT_TEST_MESSAGE( "Triangle local :\n" );
    UNIT_TEST_MESSAGE( "A : ( %s )\n", SerializeVector( v_serial, triangle[0] ) );
    UNIT_TEST_MESSAGE( "B : ( %s )\n", SerializeVector( v_serial, triangle[1] )  );
    UNIT_TEST_MESSAGE( "C : ( %s )\n\n", SerializeVector( v_serial, triangle[2] )  );

    // Creation de la matrice de transformation du triangle : local -> world

    Vector orientation  = Quaternion( UnitX(), 0.25f * Pi() );
    Vector scale        = Vector3( 2.0f, 2.0f, 2.0f );
    Vector translation  = Vector3( 1.0f, 1.0f, 1.0f );

    UNIT_TEST_MESSAGE( "Transformation dans le repere monde :\n" );
    UNIT_TEST_MESSAGE( "Orientation : %0.2f rad sur l'axe ( %s )\n", 0.25f * Pi(), SerializeVector( v_serial, UnitX() ) );
    UNIT_TEST_MESSAGE( "Echelle : ( %s )\n", SerializeVector( v_serial, scale )  );
    UNIT_TEST_MESSAGE( "Translation : ( %s )\n\n", SerializeVector( v_serial, translation )  );

    Matrix world = RMatrix( orientation );
    Scale( world, scale );
    Translate( world, translation );

    tmp = Transpose( world );

    UNIT_TEST_MESSAGE( "Matrice 'World' obtenue :\n" );
    UNIT_TEST_MESSAGE( "( %s )\n", SerializeVector( v_serial, tmp.m_column[0] ) );
    UNIT_TEST_MESSAGE( "( %s )\n", SerializeVector( v_serial, tmp.m_column[1] ) );
    UNIT_TEST_MESSAGE( "( %s )\n", SerializeVector( v_serial, tmp.m_column[2] ) );
    UNIT_TEST_MESSAGE( "( %s )\n\n", SerializeVector( v_serial, tmp.m_column[3] ) );

    // Creation de la matrice de vue : world -> camera

    Vector cam_pos  = Vector4( 1.0f, 1.0f, 3.0f );
    Vector cam_at   = world.m_column[ 3 ];          // triangle position

    UNIT_TEST_MESSAGE( "Position de la camera : ( %s )\n", SerializeVector( v_serial, cam_pos ) );
    UNIT_TEST_MESSAGE( "La camera 'regarde' a la position du triangle : ( %s )\n\n", SerializeVector( v_serial, cam_at ) );

    Vector cam_dir      = Normalize( cam_at - cam_pos );
    Vector cam_up       = UnitY();
    Vector cam_right    = Normalize( Cross( cam_dir, cam_up ) );
    cam_up              = Normalize( Cross( cam_right, cam_dir ) );

    Matrix cam_base;
    cam_base.m_column[0] = cam_right;
    cam_base.m_column[1] = cam_up;
    cam_base.m_column[2] = -cam_dir;
    cam_base.m_column[3] = cam_pos;

    tmp = Transpose( cam_base );

    UNIT_TEST_MESSAGE( "Matrice de base de la camera :\n" );
    UNIT_TEST_MESSAGE( "( %s )\n", SerializeVector( v_serial, tmp.m_column[0] ) );
    UNIT_TEST_MESSAGE( "( %s )\n", SerializeVector( v_serial, tmp.m_column[1] ) );
    UNIT_TEST_MESSAGE( "( %s )\n", SerializeVector( v_serial, tmp.m_column[2] ) );
    UNIT_TEST_MESSAGE( "( %s )\n\n", SerializeVector( v_serial, tmp.m_column[3] ) );

    Matrix view = Inverse( cam_base );

    tmp = Transpose( view );

    UNIT_TEST_MESSAGE( "Matrice 'View' obtenue par inversion de la matrice de base :\n" );
    UNIT_TEST_MESSAGE( "( %s )\n", SerializeVector( v_serial, tmp.m_column[0] ) );
    UNIT_TEST_MESSAGE( "( %s )\n", SerializeVector( v_serial, tmp.m_column[1] ) );
    UNIT_TEST_MESSAGE( "( %s )\n", SerializeVector( v_serial, tmp.m_column[2] ) );
    UNIT_TEST_MESSAGE( "( %s )\n\n", SerializeVector( v_serial, tmp.m_column[3] ) );

    // Creation de la matrice de projection de la camera : camera -> screen

    F32 near    = 1.0f;
    F32 far     = 3.0f;
    F32 width   = 4.0f;
    F32 height  = 4.0f;

    UNIT_TEST_MESSAGE( "Parametres de projection orthogonale :\n");
    UNIT_TEST_MESSAGE( "Near %0.2f, Far %0.2f, Width %0.2f, Height %0.2f\n\n", near, far, width, height );

    Matrix proj;
    proj.m_column[0] = Vector4( 2.0f / width    , 0.0f          , 0.0f                  , 0.0f  );
    proj.m_column[1] = Vector4( 0.0f            , 2.0f / height , 0.0f                  , 0.0f  );
    proj.m_column[2] = Vector4( 0.0f            , 0.0f          , 1.0f / ( near - far ) , 0.0f  );
    proj.m_column[3] = Vector4( 0.0f            , 0.0f          , near / ( near - far ) , 1.0f  );

    tmp = Transpose( proj );

    UNIT_TEST_MESSAGE( "Matrice 'Proj' obtenue :\n" );
    UNIT_TEST_MESSAGE( "( %s )\n", SerializeVector( v_serial, tmp.m_column[0] ) );
    UNIT_TEST_MESSAGE( "( %s )\n", SerializeVector( v_serial, tmp.m_column[1] ) );
    UNIT_TEST_MESSAGE( "( %s )\n", SerializeVector( v_serial, tmp.m_column[2] ) );
    UNIT_TEST_MESSAGE( "( %s )\n\n", SerializeVector( v_serial, tmp.m_column[3] ) );

    // Application de la transformation
    // coordonnees du triangle local -> coordonnees du triangle a l'ecran

    Matrix viewProj         = Mul( proj, view );
    Matrix worldViewProj    = Mul( viewProj, world );

    Vector screen_triangle[ 3 ];
    screen_triangle[ 0 ] = Mul( worldViewProj, triangle[ 0 ] );
    screen_triangle[ 1 ] = Mul( worldViewProj, triangle[ 1 ] );
    screen_triangle[ 2 ] = Mul( worldViewProj, triangle[ 2 ] );

    UNIT_TEST_MESSAGE( "Application des transformations successives ( World x View x Proj ) au triangle :\n" );
    UNIT_TEST_MESSAGE( "A en coordonnees ecran : ( %s )\n", SerializeVector( v_serial, screen_triangle[0] ) );
    UNIT_TEST_MESSAGE( "B en coordonnees ecran : ( %s )\n", SerializeVector( v_serial, screen_triangle[1] )  );
    UNIT_TEST_MESSAGE( "C en coordonnees ecran : ( %s )\n\n", SerializeVector( v_serial, screen_triangle[2] )  );

    // - Bonus -
    // Projection perspective

    F32 fov = HalfPi();         // need more ??? :)
    F32 ratio = 16.0f / 10.0f;

    F32 cotan = 1.0f / Tan( 0.5f * fov ) ;

    proj.m_column[0] = Vector4( cotan   , 0.0f          , 0.0f                  , 0.0f  );
    proj.m_column[1] = Vector4( 0.0f    , cotan * ratio , 0.0f                  , 0.0f  );
    proj.m_column[2] = Vector4( 0.0f    , 0.0f          , 1.0f / ( near - far ) , -1.0f );
    proj.m_column[3] = Vector4( 0.0f    , 0.0f          , near / ( near - far ) , 0.0f  );

    viewProj         = Mul( proj, view );
    worldViewProj    = Mul( viewProj, world );

    screen_triangle[ 0 ] = Mul( worldViewProj, triangle[ 0 ] );
    screen_triangle[ 1 ] = Mul( worldViewProj, triangle[ 1 ] );
    screen_triangle[ 2 ] = Mul( worldViewProj, triangle[ 2 ] );

    // Pour finir on retrouve les valeurs lineaires
    Vector homogeneous[ 3 ];
    homogeneous[ 0 ] = Swizzle< 3, 3, 3, 3 >( screen_triangle[ 0 ] );
    homogeneous[ 1 ] = Swizzle< 3, 3, 3, 3 >( screen_triangle[ 1 ] );
    homogeneous[ 2 ] = Swizzle< 3, 3, 3, 3 >( screen_triangle[ 2 ] );

    const Vector maskZ = Mask< 0, 0, 1, 0 >();

    homogeneous[ 0 ] = Select( homogeneous[ 0 ], One4(), maskZ );
    homogeneous[ 1 ] = Select( homogeneous[ 1 ], One4(), maskZ );
    homogeneous[ 2 ] = Select( homogeneous[ 2 ], One4(), maskZ );

    screen_triangle[ 0 ] = Div( screen_triangle[ 0 ], homogeneous[ 0 ] );
    screen_triangle[ 1 ] = Div( screen_triangle[ 1 ], homogeneous[ 1 ] );
    screen_triangle[ 2 ] = Div( screen_triangle[ 2 ], homogeneous[ 2 ] );
}
