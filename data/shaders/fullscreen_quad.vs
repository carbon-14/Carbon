#version 420

in vec4 position;

out VertexData {
    vec4 texCoord;
} VertexOut;

void main()
{
    VertexOut.texCoord = vec4( position.zw, 0.0, 1.0 );
    gl_Position = vec4( position.xy, 0.0, 1.0 );
}
