
in Data
{
    vec3 pos;
    vec2 uv;
} DataIn;

out vec4 outColor;

layout(binding=0) uniform sampler2D GBufferDepth;
layout(binding=1) uniform sampler2D GBufferNormal;
layout(binding=2) uniform sampler2D GBufferColor;

void main()
{
    GBuffer gbuffer = ReadGBuffer( GBufferDepth, GBufferNormal, GBufferColor, DataIn.uv );
    outColor = vec4(10.0*(gbuffer.depth.xxx-0.9),1.0);
}
