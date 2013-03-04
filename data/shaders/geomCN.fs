
in Data
{
    mat4    coord;
    vec2    uv;
} DataIn;

GBUFFER_NORMAL gNormal;
GBUFFER_COLOR gColor;

layout(binding=0) uniform sampler2D colorMap;
layout(binding=1) uniform sampler2D normalMap;

void main()
{
    GBuffer gbuffer;

    mat3 tbn;
    tbn[0] = normalize( DataIn.coord[0].xyz );
    tbn[1] = normalize( DataIn.coord[1].xyz );
    tbn[2] = normalize( DataIn.coord[2].xyz );

    vec3 normal = tbn * SampleNormalMap( normalMap, DataIn.uv );
    gbuffer.normal = vec4( normal, 0.0 );

    vec4 color  = SampleColorMap( colorMap, DataIn.uv );
    gbuffer.albedo = color.rgb;
    gbuffer.gloss_emissive = color.a;

    WriteGBuffer( gbuffer, gNormal, gColor );
}
