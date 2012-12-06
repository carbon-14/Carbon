#version 420

layout( location = 0 ) in vec3 position;
layout( location = 1 ) in vec3 normal;
layout( location = 5 ) in vec2 texcoord0;

out Data
{
    vec3 pos;
} DataOut;

void main()
{
    DataOut.pos = position.xyz;
    gl_Position = vec4( 0.01 * position.xy, 0.0, 1.0 );
}
