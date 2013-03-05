
VERTEX_POSITION position;
VERTEX_COLOR    color;

out Data
{
    vec4 color;
} DataOut;

void main()
{
    DataOut.color = color;

    gl_Position = ViewProjMatrix * vec4( position, 1.0 );
}
