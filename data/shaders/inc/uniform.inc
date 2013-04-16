
#define UNIFORM_BINDING_FRAME       5
#define UNIFORM_BINDING_INSTANCE    6
#define UNIFORM_BINDING_MATERIAL    7

layout(binding=UNIFORM_BINDING_FRAME) uniform FrameParameters
{
    vec4    ViewportSize;
    vec4    DepthRange;
    vec4    ViewScale;
    vec4    AmbientSkyLight;
    vec4    AmbientGroundLight;
    vec4    CameraPosition;
    mat4    ViewMatrix;
    mat4    ProjMatrix;
    mat4    ViewProjMatrix;
};

layout(binding=UNIFORM_BINDING_INSTANCE) uniform InstanceParameters
{
    mat4    WorldMatrix;
};
