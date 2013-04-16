
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

