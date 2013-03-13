
#define GBUFFER_NORMAL layout(location=0) out vec2
#define GBUFFER_COLOR layout(location=1) out vec4

struct GBuffer
{
    vec4    normal;
    vec3    albedo;
    float   mask;
    float   depth;
};

vec2 EncodeNormal( in vec4 n )
{
    return n.xy / ( 1.0 + n.z );
}

vec4 DecodeNormal( in vec2 n )
{
    vec4 normal = vec4( n, 1.0, 0.0 );
    normal /= dot( normal, normal );
    return 2.0 * normal + vec4(0.0,0.0,-1.0,0.0);
}

void WriteGBuffer( in GBuffer gbuffer, out vec2 normal, out vec4 color )
{
    normal  = EncodeNormal( gbuffer.normal );
    color   = vec4( gbuffer.albedo, gbuffer.mask );
}

GBuffer ReadGBuffer( in sampler2D depthMap, in sampler2D normalMap, in sampler2D colorMap, in vec2 uv )
{
    GBuffer gbuffer;

    vec2 n          = texture2D( normalMap, uv ).rg;
    gbuffer.normal  = DecodeNormal( n );

    vec4 color      = texture2D( colorMap, uv );
    gbuffer.albedo  = color.rgb;
    gbuffer.mask    = color.a;

    float d         = texture2D( depthMap, uv ).r;
    gbuffer.depth   = d;

    return gbuffer;
}

vec4 BuildViewPositionFromDepth( in vec2 uv, in vec4 viewScale, in float depth )
{
    vec3 pos = vec3( 2.0 * uv - 1.0, 1.0 ) * depth;
    return vec4( pos, 1.0 ) * viewScale;
}
