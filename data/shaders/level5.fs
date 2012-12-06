#version 420

in Data
{
    vec3 pos;
} DataIn;

out vec4 outColor;

void main()
{
    outColor = vec4( DataIn.pos, 1.0 );
}
