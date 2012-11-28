#version 420

in Data
{
    vec3 pos;
    vec2 uv;
} DataIn;

out vec4 outColor;

layout(binding=0) uniform sampler2D carbonColor;
layout(binding=1) uniform sampler2D carbonNormal;

const float lightIntensity = 8.0;
const float lightRadius = 1.25;
const vec3 lightPos = vec3( 0.0, 0.0, -0.75 );

const float gamma = 2.4;

void main()
{
    vec3 pos = DataIn.pos;
    vec2 uv = DataIn.uv;

    // color
    vec3 color = texture2D( carbonColor, DataIn.uv ).rgb;
    vec3 gradiant = pow( clamp( vec3( uv, 1.0-uv.x ), vec3(0.0), vec3(1.0) ), vec3(gamma) );    // level3 gradiant with sRGB manual decode
    
    color = mix( color, gradiant, 0.75 );                                                        // mix

    // normal
    vec3 normal = texture2D( carbonNormal, DataIn.uv ).rgb;

    normal.xy = 2.0 * normal.xy - 1.0;                                                          // decode normal map
    normal.z = sqrt( 1.0 - dot( normal.xy, normal.xy ) );

    // lighting compute
    vec3 ray = lightPos - pos;
    float dist = length( ray );
    vec3 dir = ray / dist;

    float lightAtt = 1.0 - min( ( dist * dist ) / ( lightRadius * lightRadius ), 1.0 );
    lightAtt *= lightAtt;
    float diffuse = max( dot( -ray, normal ), 0.0 );

    // light accumulation
    outColor.rgb = ( lightIntensity * lightAtt * diffuse ) * color;

    // sRGB manual conversion
    outColor.rgb = pow( outColor.rgb, vec3(1.0/gamma) );

    outColor.a = 1.0;
}
