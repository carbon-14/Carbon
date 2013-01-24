#version 420

layout( location = 0 ) in vec3 position;
layout( location = 1 ) in vec3 normal;
layout( location = 2 ) in vec3 tangent;
layout( location = 3 ) in vec3 binormal;
layout( location = 4 ) in vec4 color;
layout( location = 5 ) in vec2 texcoords;

layout(binding=0) uniform CameraParameters
{
    mat4    viewProjMatrix;
    vec4    cameraPosition;
};

out Data
{
    vec3 position;
    vec3 normal;
    vec3 tangent;
    vec3 binormal;
    vec3 color;
    vec2 uv;
} DataOut;

void main()
{
    DataOut.position    = position;
    DataOut.normal      = normal;
    DataOut.tangent     = tangent;
    DataOut.binormal    = binormal;
    DataOut.color       = color.rgb;
    DataOut.uv          = texcoords / 128.0;

    gl_Position         = viewProjMatrix * vec4( position, 1.0 );
}
