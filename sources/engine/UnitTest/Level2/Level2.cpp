#include "UnitTest/Level2/Level2.h"

#include "Core/Math.h"
#include "Core/Vector.h"
#include "Core/Matrix.h"

using namespace Core;

void Level2()
{
    Vector v0 = Vector4( 1.0f, 2.0f, 3.0f, 4.0f );
    Vector v1 = Vector3( 1.0f, 2.0f, 3.0f );
    Vector v2 = Vector2( 1.0f, 2.0f );
    Vector v3 = Splat( 42.0f );

    F32 a = Get( v1, 1 );
    Set( v1, 1, 42.0f );

    Vector v4 = Swizzle< 3, 2, 1, 0 >( v0 );

    Vector v5 = Dot( v1, Swizzle< 2, 1, 0, 3 >( v1 ) );
    Vector v6 = Cross3( Vector3( 1.0f, 0.0f, 0.0f ), Vector3( 0.0f, 1.0f, 0.0f ) );

    Matrix m;
}