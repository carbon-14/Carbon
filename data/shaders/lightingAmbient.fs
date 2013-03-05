
in vec2 uv;

out vec4 outColor;

layout(binding=0) uniform sampler2D GBufferDepth;
layout(binding=1) uniform sampler2D GBufferNormal;
layout(binding=2) uniform sampler2D GBufferColor;

void main()
{
    GBuffer gbuffer = ReadGBuffer( GBufferDepth, GBufferNormal, GBufferColor, uv );

    vec4 ambientLight = LightAmbient( gbuffer.normal, ViewMatrix[1], AmbientGroundLight, AmbientSkyLight );

    outColor = vec4( gbuffer.albedo, 1.0 ) * ambientLight;
}
