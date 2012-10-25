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
    Vector v3 = Splat( Math::Pi() );

    F128 a;
    Store( a, v1 );
    a[ 1 ] = 42.0f;
    v1 = Load( a );

    Vector v4 = Swizzle< 3, 2, 1, 0 >( v0 );

    Vector v5 = Dot( v1, Swizzle< 2, 1, 0, 3 >( v1 ) );
    Vector v6 = Cross( UnitX(), UnitY() );

    Matrix m = Identity();
    F128 b[ 4 ];
    Store( b, m );

    F32 * it = b[ 0 ];
    for ( int i=0; i< 16; ++i, ++it )
    {
        *it = (F32)i;
    }
    m = Load( b );

    Matrix n = Transpose( m );
    Matrix i = Inverse( m );
}
