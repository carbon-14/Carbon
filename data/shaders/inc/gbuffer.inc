
#define GBUFFER_NORMAL layout(location=0) out vec2
#define GBUFFER_COLOR layout(location=1) out vec4

struct GBuffer
{
    float   depth;
    vec3    normal;
    vec3    albedo;
    float   gloss_emissive;
};

vec2 EncodeNormal( in vec3 n )
{
    return n.xy / ( 1.0 + n.z );
}

vec3 DecodeNormal( in vec2 n )
{
    vec3 normal = vec3( n, 1.0 );
    normal /= dot(normal,normal);
    return 2.0 * normal + vec3(0.0,0.0,-1.0);
}

void WriteGBuffer( in GBuffer gbuffer, out vec2 normal, out vec4 color )
{
    normal  = EncodeNormal( gbuffer.normal );
    color   = vec4( gbuffer.albedo, gbuffer.gloss_emissive );
}

GBuffer ReadGBuffer( in sampler2D depthMap, in sampler2D normalMap, in sampler2D colorMap, in vec2 uv )
{
    GBuffer gbuffer;

    float d = texture2D( depthMap, uv ).r;
    gbuffer.depth = d;

    vec2 n = texture2D( normalMap, uv ).rg;
    gbuffer.normal = DecodeNormal( n );

    vec4 color = texture2D( colorMap, uv );
    gbuffer.albedo = color.rgb;
    gbuffer.gloss_emissive = color.a;

    return gbuffer;
}
