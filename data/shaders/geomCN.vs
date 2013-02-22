
VERTEX_POSITION     position;
VERTEX_NORMAL       normal;
VERTEX_TANGENT      tangent;
VERTEX_BINORMAL     binormal;
VERTEX_TEXCOORD0    texcoords;

out Data
{
    mat4    coord;
    vec2    uv;
} DataOut;

void main()
{
    mat4 localMatrix;
    localMatrix[0]  = vec4( tangent , 0.0 );
    localMatrix[1]  = vec4( binormal, 0.0 );
    localMatrix[2]  = vec4( normal  , 0.0 );
    localMatrix[3]  = vec4( position, 1.0 );

    DataOut.coord   = viewMatrix * worldMatrix * localMatrix;
    DataOut.uv      = texcoords / 128.0;

    gl_Position     = projMatrix * DataOut.coord[3];
}
