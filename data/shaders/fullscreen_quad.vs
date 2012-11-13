#version 420

in vec4 position;

out Data
{
    vec4 color;
} DataOut;

void main()
{
    DataOut.color = vec4( position.zw, 0.0, 1.0 );
    gl_Position = vec4( position.xy, 0.0, 1.0 );
}
