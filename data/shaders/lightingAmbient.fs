
in vec2 uv;

out vec4 outColor;

layout(binding=0) uniform sampler2D GBufferDepth;
layout(binding=1) uniform sampler2D GBufferNormal;
layout(binding=2) uniform sampler2D GBufferColor;
layout(binding=3) uniform samplerCube EnvMap;

void main()
{
    GBuffer gbuffer = ReadGBuffer( GBufferDepth, GBufferNormal, GBufferColor, uv );

    vec4 ambientLight = LightAmbient( gbuffer.normal, ViewMatrix[1], AmbientGroundLight, AmbientSkyLight );

    vec4 pos = BuildViewPositionFromDepth( uv, ViewScale, gbuffer.depth );
    vec3 v = normalize( pos.xyz );

    vec3 r = reflect( v, gbuffer.normal.xyz );

    ambientLight += texture( EnvMap, r );

    outColor = ambientLight;
}
