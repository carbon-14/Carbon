
#define DECLARE_LIGHT_PARAMETERS layout(binding=0) uniform LightParameters  \
{                                                                           \
    mat4    LightMatrix;                                                    \
    vec4    LightValueInvSqrRadius;                                         \
    vec4    LightPosition;                                                  \
    vec4    LightDirection;                                                 \
    vec4    LightSpotParameters;                                            \
}

#define LightValue          LightValueInvSqrRadius.xyz
#define LightInvSqrRadius   LightValueInvSqrRadius.w

vec4 ComputeToLight( in vec4 lightPosition, in vec4 fragPosition, out float sqrDistance )
{
    vec4 v = lightPosition - fragPosition;
    sqrDistance = dot( v, v );
    return v / sqrt( sqrDistance );
}

float LightDirectionalIntensity()
{
    return 1.0;
}

float LightOmniIntensity( in float squaredDistance, in float invSquaredRadius )
{
    float i = 1.0 - min( squaredDistance * invSquaredRadius, 1.0 );
    return i * i;
}

float LightSpotIntensity( in float squaredDistance, in float invSquaredRadius, in vec4 spotDirection, in vec4 toLight, in vec2 spotParams )
{
    float i = 1.0 - min( squaredDistance * invSquaredRadius, 1.0 );

    i *=  clamp( dot( spotDirection, -toLight ) * spotParams.x + spotParams.y, 0.0, 1.0 );
    
    return i * i;
}

vec4 LightAmbient( in vec4 normal, in vec4 viewY, in vec4 groundLight, in vec4 skyLight )
{
    float ratio = dot( normal, viewY );
    return mix( skyLight, groundLight, 0.5 * ratio + 0.5 );
}

float DiffuseTerm( in vec4 normal, in vec4 toLight )
{
    return max( dot( normal, toLight ), 0.0 );
}
