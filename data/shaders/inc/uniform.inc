
#define UNIFORM_BINDING_FRAME       5
#define UNIFORM_BINDING_INSTANCE    6
#define UNIFORM_BINDING_MATERIAL    7

layout(binding=UNIFORM_BINDING_FRAME) uniform FrameParameters
{
    vec4    viewportSize;
    vec4    depthRange;
    vec4    viewScale;
    mat4    viewMatrix;
    mat4    projMatrix;
    mat4    viewProjMatrix;
};

layout(binding=UNIFORM_BINDING_INSTANCE) uniform InstanceParameters
{
    mat4    worldMatrix;
};
