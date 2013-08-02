
in vec2 uv;

out vec4 outColor;

layout(binding=0) uniform sampler2D GBufferDepth;
layout(binding=1) uniform sampler2D GBufferNormal;
layout(binding=2) uniform sampler2D GBufferColor;
layout(binding=3) uniform samplerCube EnvMap;
layout(binding=4) uniform samplerCube EnvMapBlur;
layout(binding=5) uniform sampler2D FinalBuffer;

void main()
{
	vec4 color = texture( FinalBuffer, uv );

    /*//if ( uv.x < 1.0/3.0 && uv.y < 1.0/3.0 )
    {
        const vec2 angles = vec2( 6.28318530718, 3.14159265359 ) * ( vec2(0.5,1.0) + uv );

        vec4 sincos = vec4( sin( angles ), cos( angles ) );

        vec3 coords;
        coords.z = sincos.z * sincos.y;
        coords.x = sincos.x * sincos.y;
        coords.y = sincos.w;
        outColor = texture( EnvMapBlur, coords );
    }
    else*/
    {
        outColor = texture( FinalBuffer, uv );
    }
}
