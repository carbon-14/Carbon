
#define UNIFORM_BINDING_SCENE       5
#define UNIFORM_BINDING_INSTANCE    6
#define UNIFORM_BINDING_IMATERIAL   7

layout(binding=UNIFORM_BINDING_SCENE) uniform SceneParameters
{
    mat4    viewMatrix;
    mat4    projMatrix;
    vec4    cameraPosition;
};

layout(binding=UNIFORM_BINDING_INSTANCE) uniform InstanceParameters
{
    mat4    worldMatrix;
};
