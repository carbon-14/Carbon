#version 420

in Data
{
    vec3 position;
    vec3 normal;
    vec3 color;
} DataIn;

out vec4 outColor;

float rad = 20.0;

void main()
{
    float d = length(DataIn.position);
    vec3 l = -DataIn.position / d;
    vec3 n = normalize(DataIn.normal);

    float diff = dot( l, n );

    float att = max( 1.0 - d * d / ( rad * rad ), 0.0 );

    vec3 ambient = 0.1 * DataIn.color.rgb * pow( mix( vec3( 137.0, 175.0, 220.0 ), vec3( 103.0, 98.0, 94.0 ), 0.5 + 0.5 * DataIn.normal.yyy ) / 255.0, vec3(2.4) );

    outColor = vec4( att * att * diff.xxx + ambient, 1.0 );
}
