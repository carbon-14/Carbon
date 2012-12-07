#version 420

layout( location = 0 ) in vec4 position;
layout( location = 1 ) in vec4 normal;

out vec4 test;

void main()
{
    test = normal;
    gl_Position = position;
}
