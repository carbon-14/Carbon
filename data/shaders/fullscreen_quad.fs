#version 420

in Data
{
    vec4 color;
} DataIn;

out vec4 color;

void main()
{
    color = DataIn.color;
}
