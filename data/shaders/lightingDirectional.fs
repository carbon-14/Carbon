
out vec4 outColor;

DECLARE_LIGHT_PARAMETERS;

layout(binding=0) uniform sampler2D GBufferDepth;
layout(binding=1) uniform sampler2D GBufferNormal;
layout(binding=2) uniform sampler2D GBufferColor;

void main()
{
    vec2 uv = gl_FragCoord.xy * viewportSize.zw;

    GBuffer gbuffer = ReadGBuffer( GBufferDepth, GBufferNormal, GBufferColor, uv );

    vec3 l = LightValue * max( dot( LightDirection.xyz, gbuffer.normal ), 0.0 );

    outColor = vec4( gbuffer.albedo * l, 1.0 );
}
