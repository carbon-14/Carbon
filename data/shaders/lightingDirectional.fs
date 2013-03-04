
out vec4 outColor;

DECLARE_LIGHT_PARAMETERS;

layout(binding=0) uniform sampler2D GBufferDepth;
layout(binding=1) uniform sampler2D GBufferNormal;
layout(binding=2) uniform sampler2D GBufferColor;

void main()
{
    vec2 uv = gl_FragCoord.xy * ViewportSize.zw;

    GBuffer gbuffer = ReadGBuffer( GBufferDepth, GBufferNormal, GBufferColor, uv );

    float intensity = LightDirectionalIntensity();

    float diffuse_term = DiffuseTerm( gbuffer.normal, -LightDirection );

    vec3 lightColor = LightValue * intensity;

    outColor = vec4( gbuffer.albedo * lightColor * diffuse_term, 1.0 );
}
