
#define DECLARE_LIGHT_PARAMETERS layout(binding=0) uniform LightParameters  \
{                                                                           \
    mat4    LightMatrix;                                                    \
    vec4    LightValueInvSqrRadius;                                         \
    vec4    LightPosition;                                                  \
    vec4    LightDirection;                                                 \
}

#define LightValue          LightValueInvSqrRadius.xyz
#define LightInvSqrRadius   LightValueInvSqrRadius.w
