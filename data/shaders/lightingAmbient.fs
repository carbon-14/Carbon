
in vec2 uv;

out vec4 outColor;

layout(binding=0) uniform sampler2D GBufferDepth;
layout(binding=1) uniform sampler2D GBufferNormal;
layout(binding=2) uniform sampler2D GBufferColor;
layout(binding=3) uniform samplerCube EnvMap;

void main()
{
    GBuffer gbuffer = ReadGBuffer( GBufferDepth, GBufferNormal, GBufferColor, uv );

    vec4 pos = BuildViewPositionFromDepth( uv, ViewScale, gbuffer.depth );

    vec4 v = normalize( vec4(pos.xyz, 0.0) );

    vec4 r = reflect( v, gbuffer.normal );

    /*const float scale = 1.77777;

    vec2 n = 2.0 * uv - vec2(1.0);
    vec4 caca = vec4( n, -1.0, 0.0 ) * scale;
    caca /= dot( caca, caca );
    caca = 2.0 * caca + vec4(0.0,0.0,1.0,0.0);*/

    outColor = texture( EnvMap, r.xyz );
    //outColor = vec4(0.0);
}
