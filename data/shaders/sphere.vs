
layout( location = 0 ) in vec3 position;
layout( location = 1 ) in vec3 normal;
layout( location = 2 ) in vec3 tangent;
layout( location = 3 ) in vec3 binormal;
layout( location = 5 ) in vec2 texcoords;

layout(binding=0) uniform CameraParameters
{
    mat4    viewProjMat;
    vec4    camPosition;
};

layout(binding=4) uniform SphereParameters
{
    mat4    worldMat;
    vec4    emissiveColor;
};

out Data
{
    vec3 position;
    vec3 normal;
    vec3 tangent;
    vec3 binormal;
    vec2 uv;
} DataOut;

void main()
{
    vec4 p = worldMat * vec4( position, 1.0 );
    mat3 world33 = mat3( worldMat[0].xyz, worldMat[1].xyz, worldMat[2].xyz );

    DataOut.position    = p.xyz;
    DataOut.normal      = world33 * normal;
    DataOut.tangent     = world33 * tangent;
    DataOut.binormal    = world33 * binormal;
    DataOut.uv          = texcoords / 128.0;

    gl_Position         = viewProjMat * p;
}
