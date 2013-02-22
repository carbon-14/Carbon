
in vec4 position;

out Data
{
    vec3 pos;
    vec2 uv;
} DataOut;

void main()
{
    DataOut.pos = vec3( position.xy, 0.0 );
    DataOut.uv = position.zw;
    
    gl_Position = vec4( position.xy, 0.0, 1.0 );
}
