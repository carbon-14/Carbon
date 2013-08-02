
in vec2 uv;

out vec4 outColor;

layout(binding=0) uniform sampler2D GBufferDepth;
layout(binding=1) uniform sampler2D GBufferNormal;
layout(binding=2) uniform sampler2D GBufferColor;
layout(binding=3) uniform samplerCube EnvMap;

const vec4 BoxCenter = vec4( 0.0, 0.0, 0.0, 1.0 );
const vec4 BoxSize = vec4( 19.7, 15.25, 7.4, 1.0 );

void main()
{
    GBuffer gbuffer = ReadGBuffer( GBufferDepth, GBufferNormal, GBufferColor, uv );

    vec4 pos    = BuildViewPositionFromDepth( uv, ViewScale, gbuffer.depth );
    vec4 n      = gbuffer.normal;
    vec4 v      = normalize( vec4(pos.xyz, 0.0) );
    vec4 r      = reflect( v, n );

    vec4 spec   = texture( EnvMap, r.xyz );

    float i     = mix( 1.0, pow( 1.0 - dot( -v, n ), 5.0 ), FresnelCoeff );
    
    outColor = i * spec;
}
