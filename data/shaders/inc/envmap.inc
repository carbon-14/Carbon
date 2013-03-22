
#define DECLARE_ENV_BLUR_PARAMETERS             \
layout(binding = 0) uniform envBlurParameters   \
{                                               \
    float   BlurWeights[65];                    \
}

const int EnvMapImageSize       = 256;
const int EnvMapImageRingSize   = EnvMapImageSize * 4;
const int EnvMapBlurRadius      = 32;
const int EnvMapBlurSize        = 2 * EnvMapBlurRadius + 1;

const int EnvMapFaceMapping[4]  = { 0, 2, 1, 3 };
const int EnvMapCoordMax        = EnvMapImageSize - 1;

ivec4 ComputeEnvMapImageCoordsXY( ivec3 globalID )
{
    ivec2 uv;
    if      ( globalID.z == 0 ) { uv = ivec2( 0, EnvMapCoordMax ) + ivec2( +1, -1 ) * globalID.yx; }
    else if ( globalID.z == 1 ) { uv = ivec2( EnvMapCoordMax, 0 ) + ivec2( -1, +1 ) * globalID.yx; }
    else if ( globalID.z == 2 ) { uv = ivec2( EnvMapCoordMax ) - globalID.xy; }
    else if ( globalID.z == 3 ) { uv = globalID.xy; }

    int face = globalID.z;
    int index = globalID.x + EnvMapFaceMapping[ globalID.z ] * EnvMapImageSize;

    return ivec4( uv, face, index );
}

ivec4 ComputeEnvMapImageCoordsYZ( ivec3 globalID )
{
    ivec2 uv;
    if ( globalID.z == 3 )  { uv = ivec2( 0, EnvMapCoordMax ) + ivec2( +1, -1 ) * globalID.yx; }
    else                    { uv = ivec2( EnvMapCoordMax, 0 ) + ivec2( -1, +1 ) * globalID.yx; }

    int face = globalID.z + 2;
    int index = globalID.x + EnvMapFaceMapping[ globalID.z ] * EnvMapImageSize;

    return ivec4( uv, face, index );
}

ivec4 ComputeEnvMapImageCoordsZX( ivec3 globalID )
{
    ivec2 uv = globalID.xy;

    int face = ( globalID.z + 4 ) % 6;
    int index = globalID.x + EnvMapFaceMapping[ globalID.z ] * EnvMapImageSize;

    return ivec4( uv, face, index );
}

ivec3 ComputeEnvMapImageCoordsLeft( ivec3 globalID )
{
    ivec3 coords;

    if      ( globalID.z == 0 ) { coords = ivec3( globalID.xy*ivec2(+1,+1)+ivec2(223,0), 4 ); }
    else if ( globalID.z == 1 ) { coords = ivec3( globalID.xy*ivec2(+1,+1)+ivec2(223,0), 5 ); }
    else if ( globalID.z == 2 ) { coords = ivec3( globalID.yx*ivec2(+1,-1)+ivec2(0,31), 1 ); }
    else if ( globalID.z == 3 ) { coords = ivec3( globalID.yx*ivec2(-1,+1)+ivec2(255,223), 1 ); }
    else if ( globalID.z == 4 ) { coords = ivec3( globalID.yx*ivec2(+1,+1)+ivec2(0,223), 2 ); }
    else if ( globalID.z == 5 ) { coords = ivec3( globalID.yx*ivec2(-1,-1)+ivec2(255,32), 2 ); }

    return coords;
}

ivec3 ComputeEnvMapImageCoordsCenter( ivec3 globalID )
{
    ivec3 coords;

    if      ( globalID.z == 0 ) { coords = ivec3( globalID.xy*ivec2(+1,+1)+ivec2(-32,0), 0 ); }
    else if ( globalID.z == 1 ) { coords = ivec3( globalID.xy*ivec2(+1,+1)+ivec2(-32,0), 1 ); }
    else if ( globalID.z == 2 ) { coords = ivec3( globalID.xy*ivec2(+1,+1)+ivec2(-32,0), 2 ); }
    else if ( globalID.z == 3 ) { coords = ivec3( globalID.xy*ivec2(+1,+1)+ivec2(-32,0), 3 ); }
    else if ( globalID.z == 4 ) { coords = ivec3( globalID.yx*ivec2(+1,+1)+ivec2(0,-32), 4 ); }
    else if ( globalID.z == 5 ) { coords = ivec3( globalID.yx*ivec2(+1,+1)+ivec2(0,-32), 5 ); }

    return coords;
}

ivec3 ComputeEnvMapImageCoordsRight( ivec3 globalID )
{
    ivec3 coords;

    if      ( globalID.z == 0 ) { coords = ivec3( globalID.xy*ivec2(+1,+1)+ivec2(-288,0), 5 ); }
    else if ( globalID.z == 1 ) { coords = ivec3( globalID.xy*ivec2(+1,+1)+ivec2(-288,0), 4 ); }
    else if ( globalID.z == 2 ) { coords = ivec3( globalID.yx*ivec2(-1,+1)+ivec2(255,-288), 0 ); }
    else if ( globalID.z == 3 ) { coords = ivec3( globalID.yx*ivec2(+1,-1)+ivec2(0,543), 0 ); }
    else if ( globalID.z == 4 ) { coords = ivec3( globalID.yx*ivec2(+1,+1)+ivec2(0,-288), 3 ); }
    else if ( globalID.z == 5 ) { coords = ivec3( globalID.yx*ivec2(-1,-1)+ivec2(255,543), 3 ); }

    return coords;
}
