
in vec4 position;

DECLARE_LIGHT_PARAMETERS;

void main()
{
    gl_Position = LightMatrix * position;
}
