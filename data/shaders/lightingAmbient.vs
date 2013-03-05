
in vec4 position;

out vec2 uv;

void main()
{
    uv = position.zw;
    
    gl_Position = vec4( position.xy, 1.0, 1.0 );
}
