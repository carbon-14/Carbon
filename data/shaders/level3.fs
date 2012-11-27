#version 420

in Data
{
    vec4 color;
} DataIn;

out vec4 outColor;

void main()
{
    outColor = DataIn.color;
}
