
vec4 SampleColorMap( in sampler2D colorMap, in vec2 uv )
{
    return texture2D( colorMap, uv );
}

vec3 SampleNormalMap( in sampler2D normalMap, in vec2 uv )
{
    vec3 n;

    n.xy = texture2D( normalMap, uv ).rg;
    n.xy = n.xy * 2.0 - 1.0;
    n.z = sqrt( 1.0 - dot(n.xy,n.xy) );

    return n;
}

