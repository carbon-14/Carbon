#version 420

in Data
{
    vec3 pos;
    vec2 uv;
} DataIn;

out vec4 outColor;

layout(binding=0) uniform sampler2D carbonColor;
layout(binding=1) uniform sampler2D carbonNormal;

const vec3 bgColor = vec3( 162.0, 14.0, 2.0 ) / 255.0;

const float lightIntensity = 8.0;
const float lightRadius = 1.25;
const vec3 lightPos = vec3( 0.0, 0.0, 0.75 );

const float gamma = 2.4;

void main()
{
    vec3 pos = DataIn.pos;
    vec2 uv = DataIn.uv;

    // color
    vec3 color = texture2D( carbonColor, DataIn.uv ).rgb;
    vec3 colorize = pow( bgColor, vec3(gamma) );                    // sRGB manual decode
    
    color = mix( color, colorize, 0.5 );                            // mix

    // normal
    vec3 normal = texture2D( carbonNormal, DataIn.uv ).rgb;

    normal.xy = vec2( 2.0, -2.0 ) * normal.xy + vec2( -1.0, 1.0 );  // decode normal map
    normal.z = sqrt( 1.0 - dot( normal.xy, normal.xy ) );

    // lighting compute
    vec3 ray = lightPos - pos;
    float dist = length( ray );
    vec3 dir = ray / dist;

    float lightAtt = 1.0 - min( ( dist * dist ) / ( lightRadius * lightRadius ), 1.0 );
    lightAtt *= lightAtt;
    float diffuse = max( dot( ray, normal ), 0.0 );

    // light accumulation
    outColor.rgb = ( lightIntensity * lightAtt * diffuse ) * color;

    outColor.a = 1.0;
}
