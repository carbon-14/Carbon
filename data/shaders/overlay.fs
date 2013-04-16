
in vec2 uv;

out vec4 outColor;

layout(binding=0) uniform sampler2D GBufferDepth;
layout(binding=1) uniform sampler2D GBufferNormal;
layout(binding=2) uniform sampler2D GBufferColor;
layout(binding=3) uniform samplerCube EnvMap;
layout(binding=4) uniform samplerCube EnvMapBlur;
layout(binding=5) uniform sampler2D LightTexture;

#define OVERLAYID 3

void main()
{
#if OVERLAYID == 0
    if ( uv.x < 0.5 && uv.y < 0.5 ) {
        vec2 coords = 2.0 * uv;
        outColor = vec4( texture( GBufferColor, coords ).rgb, 1.0 );
    } else if ( uv.x < 0.5 ) {
        vec2 coords = 2.0 * uv + vec2(-1.0,0.0);
        outColor = vec4( texture( GBufferDepth, coords ).rrr, 1.0 );
    } else if ( uv.y < 0.5 ) {
        vec2 coords = 2.0 * uv + vec2(0.0,-1.0);
        outColor = texture( LightTexture, coords );
    } else {
        vec2 coords = 2.0 * uv + vec2(-1.0);
        vec2 n = texture( GBufferNormal, coords ).rg;
        outColor = vec4( DecodeNormal( n ).rgb, 1.0 );
    }
#elif OVERLAYID == 1
    outColor = texture( LightTexture, uv );
#elif OVERLAYID == 2
    outColor = texture( LightTexture, uv );

    if ( uv.x < 1.0/3.0 && uv.y < 1.0/3.0 )
    {
        const vec2 angles = vec2( 6.28318530718, 3.14159265359 ) * ( vec2(0.5,1.0) + vec2(-3.0,3.0) * uv );

        vec4 sincos = vec4( sin( angles ), cos( angles ) );

        vec3 coords;
        coords.z = sincos.z * sincos.y;
        coords.x = sincos.x * sincos.y;
        coords.y = sincos.w;
        outColor = texture( EnvMap, coords );
    }
#elif OVERLAYID == 3
    outColor = texture( LightTexture, uv );

    if ( uv.x < 1.0/3.0 && uv.y < 1.0/3.0 )
    {
        const vec2 angles = vec2( 6.28318530718, 3.14159265359 ) * ( vec2(0.5,1.0) + vec2(-3.0,3.0) * uv );

        vec4 sincos = vec4( sin( angles ), cos( angles ) );

        vec3 coords;
        coords.z = sincos.z * sincos.y;
        coords.x = sincos.x * sincos.y;
        coords.y = sincos.w;
        outColor = texture( EnvMapBlur, coords );
    }
#endif
}
