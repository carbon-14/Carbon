
out vec4 outColor;

DECLARE_LIGHT_PARAMETERS;

layout(binding=0) uniform sampler2D GBufferDepth;
layout(binding=1) uniform sampler2D GBufferNormal;
layout(binding=2) uniform sampler2D GBufferColor;

void main()
{
    vec2 uv = gl_FragCoord.xy * ViewportSize.zw;

    GBuffer gbuffer = ReadGBuffer( GBufferDepth, GBufferNormal, GBufferColor, uv );

    vec4 pos = BuildViewPositionFromDepth( uv, ViewScale, gbuffer.depth );

    float sqr;
    vec4 L = ComputeToLight( LightPosition, pos, sqr );

    float intensity = LightOmniIntensity( sqr, LightInvSqrRadius );

    float diffuse = DiffuseTerm( gbuffer.normal, L );

    vec3 lightColor = LightValue * intensity;

    outColor = vec4( gbuffer.albedo * lightColor * diffuse, 1.0 );
}
