#version 420

layout( location = 0 ) in vec3 position;
layout( location = 4 ) in vec4 color;
layout( location = 5 ) in vec2 texcoord0;

out vec2 uvs;
out vec4 test;

void main()
{
    uvs = texcoord0 / 128.0 ;
    test = color;
    gl_Position = vec4(position,1.0);
}
