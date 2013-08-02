
layout (local_size_x = 16, local_size_y = 16) in;

layout(binding=0)           uniform sampler2D GBufferColor;
layout(binding=0, r32f)     readonly uniform image2D GBufferDepth;
layout(binding=1, rg16f)    readonly uniform image2D GBufferNormal;
layout(binding=2, rgba16f)	writeonly uniform image2D lightImage;

layout(binding = 0) readonly buffer TiledDepthBufferBlock
{
    uint tiledDepthBuffer[];
};

struct LightInfos
{
	vec4	valueInvSqrRadius;
    vec4	position;
    vec4	direction;
    vec4	spotParameters;
    uint    depth;
    uint	flags;
    uint	quadId;
};

layout(binding = 1) readonly buffer LightInfosBufferBlock
{
	LightInfos  lightInfosBuffer[];
};

layout(binding=0) uniform LightingParameters
{
    uint	LightCount;
};

shared uint minDepth;
shared uint maxDepth;
shared uint lightsXY[256];
shared uint lightsXYCount;
shared uint lightsZ[64][3];
shared uint lightsZCount[3];

void main()
{
    const uint tileId2D = gl_WorkGroupID.x + gl_NumWorkGroups.x * gl_WorkGroupID.y;
    const uint workGroupSize = gl_WorkGroupSize.x * gl_WorkGroupSize.y;
    const ivec2 coord = ivec2(gl_GlobalInvocationID.xy);
    const vec2 uv = (vec2(coord) + vec2(0.5,0.5))*ViewportSize.zw;

    GBuffer gbuffer;

    vec2 n          = imageLoad( GBufferNormal, coord ).xy;
    gbuffer.normal  = DecodeNormal( n );

    vec4 color      = texelFetch( GBufferColor, coord, 0 );
    gbuffer.albedo  = color.rgb;
    gbuffer.mask    = color.a;

    float d         = imageLoad( GBufferDepth, coord );
    gbuffer.depth   = d;
    
    vec4 pos        = BuildViewPositionFromDepth( uv, ViewScale, gbuffer.depth );

    vec4 N          = gbuffer.normal;
    vec4 V          = normalize(vec4(-pos.xyz, 0.0));

    float dotVN     = dot( V, N );
    vec4 projVN     = dotVN * N;
    float angleVN   = acos( dotVN );
    vec4 projVP     = normalize( V - projVN );

    float viewProj = clamp( dotVN, 0.0, 1.0 );
    float fresnelTerm = mix( 1.0, pow( 1.0 - viewProj, 5.0 ), FresnelCoeff );

    vec3 ambient = LightAmbient( N, ViewMatrix[1], AmbientGroundLight, AmbientSkyLight ).rgb;
    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);

    if ( gl_LocalInvocationIndex == 0 )
    {
        uint tileDepth = tiledDepthBuffer[ tileId2D ];
        minDepth = tileDepth & 0x0000ffff;
        maxDepth = tileDepth >> 16;

        lightsXYCount = 0;
        lightsZCount[0] = 0;
        lightsZCount[1] = 0;
        lightsZCount[2] = 0;
    }

    barrier();

    for ( uint i=gl_LocalInvocationIndex; i<LightCount; i += workGroupSize )
    {
        LightInfos infos = lightInfosBuffer[ i ];

        uint quadId = infos.quadId;

        uvec4 quad;
        quad.x = 0x000000ff & quadId;
        quad.y = 0x000000ff & ( quadId >> 8 );
        quad.z = 0x000000ff & ( quadId >> 16 );
        quad.w = 0x000000ff & ( quadId >> 24 );

        bvec4 t = lessThan( gl_WorkGroupID.xxyy, quad );
        t = t && !t.yxwz;

        if ( t.y && t.w )
        {
            lightsXY[ atomicAdd( lightsXYCount, 1 ) ] = i;
        }
    }

    barrier();

    for ( uint i=gl_LocalInvocationIndex; i<lightsXYCount; i += workGroupSize )
    {
        uint lightId = lightsXY[ i ];
        uint depth = lightInfosBuffer[ lightId ].depth;

        uint lightMinDepth = depth & 0x0000ffff;
        uint lightMaxDepth = depth >> 16;

        if ( lightMinDepth < maxDepth && lightMaxDepth > minDepth )
        {
            uint type = lightInfosBuffer[ lightId ].flags;
            uint index = atomicAdd( lightsZCount[type], 1 );
            lightsZ[ index ][ type ] = lightId;
        }
    }

    barrier();

    for ( uint i=0; i<lightsZCount[0]; ++i )
    {
        uint lightId = lightsZ[ i ][0];
        LightInfos infos = lightInfosBuffer[ lightId ];

        vec4 L = -infos.direction;
        
        float dotLN     = dot( L, N );

        ////////////////////////////////////////////////////////////////////////////////
        if ( dotLN > 0.0 )
        {
            float sqrDistance = dot( L, L );
            float invDistance = 1.0 / sqrt( sqrDistance );
            L *= invDistance;
            dotLN *= invDistance;
            
            float angleLN   = acos( clamp(dotLN,-1.0,1.0) );
            float alpha     = max(angleVN,angleLN);
            float beta      = min(angleVN,angleLN);
            float C         = sin(alpha) * tan(beta);

            vec4 projLN     = dotLN * N;
            vec4 projLP     = normalize( L - projLN );
            float gamma     = max( dot( projVP, projLP ), 0.0 );

            float diffuseTerm = dotLN * ( RoughnessA + RoughnessB * C * gamma );

            ////////////////////////////////////////////////////////////////////////////////

            vec4 H          = normalize( L + V );
            float dotHN     = dot( H, N );
            float dotHV     = dot( H, V );

            float geo = min( dotVN, dotLN );
            geo = min( 2.0 * geo * dotHN / dotHV, 1.0 );

            float dotHN2 = dotHN * dotHN;
            float dotHN4 = dotHN2 * dotHN2;

            float r = exp( ( dotHN2 - 1.0 ) / ( SqrRoughness * dotHN2 ) ) / ( SqrRoughness * dotHN4 );

            float specularTerm = geo * r / dotVN;

            ////////////////////////////////////////////////////////////////////////////////

            vec3 intensity = infos.valueInvSqrRadius.xyz;

            diffuse += diffuseTerm * intensity;
            specular += specularTerm * intensity;
        }
    }

    for ( uint i=0; i<lightsZCount[1]; ++i )
    {
        uint lightId = lightsZ[ i ][1];
        LightInfos infos = lightInfosBuffer[ lightId ];

        vec4 L = infos.position - pos;
        
        float dotLN     = dot( L, N );

        ////////////////////////////////////////////////////////////////////////////////
        if ( dotLN > 0.0 )
        {
            float sqrDistance = dot( L, L );
            float invDistance = 1.0 / sqrt( sqrDistance );
            L *= invDistance;
            dotLN *= invDistance;
            
            float angleLN   = acos( clamp(dotLN,-1.0,1.0) );
            float alpha     = max(angleVN,angleLN);
            float beta      = min(angleVN,angleLN);
            float C         = sin(alpha) * tan(beta);

            vec4 projLN     = dotLN * N;
            vec4 projLP     = normalize( L - projLN );
            float gamma     = max( dot( projVP, projLP ), 0.0 );

            float diffuseTerm = dotLN * ( RoughnessA + RoughnessB * C * gamma );

            ////////////////////////////////////////////////////////////////////////////////

            vec4 H          = normalize( L + V );
            float dotHN     = dot( H, N );
            float dotHV     = dot( H, V );

            float geo = min( dotVN, dotLN );
            geo = min( 2.0 * geo * dotHN / dotHV, 1.0 );

            float dotHN2 = dotHN * dotHN;
            float dotHN4 = dotHN2 * dotHN2;

            float r = exp( ( dotHN2 - 1.0 ) / ( SqrRoughness * dotHN2 ) ) / ( SqrRoughness * dotHN4 );

            float specularTerm = geo * r / dotVN;

            ////////////////////////////////////////////////////////////////////////////////

            vec3 intensity = infos.valueInvSqrRadius.xyz * LightOmniIntensity( sqrDistance, infos.valueInvSqrRadius.w );

            diffuse += diffuseTerm * intensity;
            specular += specularTerm * intensity;
        }
    }

    for ( uint i=0; i<lightsZCount[2]; ++i )
    {
        uint lightId = lightsZ[ i ][2];
        LightInfos infos = lightInfosBuffer[ lightId ];

        vec4 L = infos.position - pos;
        
        float dotLN     = dot( L, N );

        ////////////////////////////////////////////////////////////////////////////////
        if ( dotLN > 0.0 )
        {
            float sqrDistance = dot( L, L );
            float invDistance = 1.0 / sqrt( sqrDistance );
            L *= invDistance;
            dotLN *= invDistance;
            
            float angleLN   = acos( clamp(dotLN,-1.0,1.0) );
            float alpha     = max(angleVN,angleLN);
            float beta      = min(angleVN,angleLN);
            float C         = sin(alpha) * tan(beta);

            vec4 projLN     = dotLN * N;
            vec4 projLP     = normalize( L - projLN );
            float gamma     = max( dot( projVP, projLP ), 0.0 );

            float diffuseTerm = dotLN * ( RoughnessA + RoughnessB * C * gamma );

            ////////////////////////////////////////////////////////////////////////////////

            vec4 H          = normalize( L + V );
            float dotHN     = dot( H, N );
            float dotHV     = dot( H, V );

            float geo = min( dotVN, dotLN );
            geo = min( 2.0 * geo * dotHN / dotHV, 1.0 );

            float dotHN2 = dotHN * dotHN;
            float dotHN4 = dotHN2 * dotHN2;

            float r = exp( ( dotHN2 - 1.0 ) / ( SqrRoughness * dotHN2 ) ) / ( SqrRoughness * dotHN4 );

            float specularTerm = geo * r / dotVN;

            ////////////////////////////////////////////////////////////////////////////////

            vec3 intensity = infos.valueInvSqrRadius.xyz * LightSpotIntensity( sqrDistance, infos.valueInvSqrRadius.w, infos.direction, L, infos.spotParameters.xy );

            diffuse += diffuseTerm * intensity;
            specular += specularTerm * intensity;
        }
    }

    vec3 light = gbuffer.albedo * ( ambient + diffuse ) + gbuffer.mask * specular * fresnelTerm;
    //light = vec3(lightsZCount[2]) / 32.0;

    imageStore( lightImage, coord, vec4(light,1.0) );
}
