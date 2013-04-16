
layout (local_size_x = 256, local_size_y = 1, local_size_z = 4) in;

DECLARE_ENV_BLUR_PARAMETERS;

layout(binding = 0, rgba16f) uniform imageCube img;

shared vec4 colorCache[EnvMapImageRingSize];

void main()
{
    ivec4 coords = ComputeEnvMapImageCoordsXY( ivec3(gl_GlobalInvocationID) );
    colorCache[ coords.w ] = imageLoad(img, coords.xyz);

    barrier();

    vec4 color = vec4(0.0);
    int index = coords.w + EnvMapImageRingSize - EnvMapBlurRadius;
    for ( int i=0; i<EnvMapBlurSize; ++i, ++index )
    {
        color += BlurWeights[ i ] * colorCache[ index % EnvMapImageRingSize ];
    }

    imageStore(img, coords.xyz, color);
}
