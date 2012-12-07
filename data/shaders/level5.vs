#version 420

layout( location = 0 ) in vec3 position;
layout( location = 1 ) in vec3 normal;
layout( location = 4 ) in vec4 color;

out Data
{
    vec3 position;
    vec3 normal;
    vec3 color;
} DataOut;

const float zrange = 500.0;
const float zoom = 1.0 / 32.0;
const vec3 proj = vec3( 1.0, 1280.0 / 720.0, -2.0 / zrange ) * vec3( zoom, zoom, 1.0 );

const float scale = 1.0;
const vec3 translation = vec3( 0.0, 0.0, 0.0 );

void main()
{
    DataOut.position    = position * scale + translation;
    DataOut.normal      = normal;
    DataOut.color       = color.rgb;

    vec3 pos_proj = ( DataOut.position ) * proj - vec3( 0.0, 0.0, 1.0 );

    gl_Position = vec4( pos_proj, 1.0);
}
