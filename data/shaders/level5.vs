#version 420

layout( location = 0 ) in vec3 position;
layout( location = 1 ) in vec3 normal;
layout( location = 4 ) in vec4 color;
layout( location = 5 ) in vec2 texcoords;

out Data
{
    vec3 position;
    vec3 normal;
    vec3 color;
    vec2 uv;
} DataOut;

const float zrange = 500.0;
const float zoom = 1.0 / 32.0;
const vec3 proj = vec3( 1.0, 1280.0 / 720.0, -2.0 / zrange ) * vec3( zoom, zoom, 1.0 );

const vec3 camPos = vec3( 0.0, 0.0, 5.0 );

void main()
{
    DataOut.position    = position;
    DataOut.normal      = normal;
    DataOut.color       = color.rgb;
    DataOut.uv          = texcoords / 128.0;

    vec3 pos_proj = ( position - camPos ) * proj - vec3( 0.0, 0.0, 1.0 );

    gl_Position = vec4( pos_proj, 1.0 );
}
