#version 420

layout( location = 0 ) in vec3 position;
layout( location = 1 ) in vec3 normal;
layout( location = 4 ) in vec4 color;
layout( location = 5 ) in vec2 texcoords;

layout(binding=0) uniform ParameterBlock
{
    mat4    viewProjMatrix;
    float   time;
};

out Data
{
    vec3 position;
    vec3 normal;
    vec3 color;
    vec2 uv;
} DataOut;

void main()
{
    DataOut.position    = position;
    DataOut.normal      = normal;
    DataOut.color       = color.rgb;
    DataOut.uv          = texcoords / 128.0;

    gl_Position         = viewProjMatrix * vec4( position, 1.0 );
}
