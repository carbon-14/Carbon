#version 420

in Data
{
    vec3 position;
    vec3 normal;
    vec3 tangent;
    vec3 binormal;
    vec3 color;
    vec2 uv;
} DataIn;

out vec4 outColor;

layout(binding=0) uniform sampler2D carbonColor;
layout(binding=1) uniform sampler2D carbonNormal;

layout(binding=0) uniform ParameterBlock
{
    mat4    viewProjMatrix;
    float   time;
};

vec3 Gamma( in vec3 c ) { return pow( c, vec3(2.4) ); }

const vec3 lightPosition = vec3( 10.0, 0.0, 0.0 );
const float lightIntensity = 0.5;
const vec3 lightColor = Gamma( vec3( 255.0, 220.0, 200.0 ) / 255.0 );
const float lightRadius = 20.0;

const vec3 skyColor = Gamma( vec3( 160.0, 200.0, 255.0 ) / 255.0 );
const vec3 gndColor = 0.01 * Gamma( vec3( 255.0, 255.0, 255.0 ) / 255.0 );
const float aoIntensity = 1.0;
const float ambient = 0.5;

void main()
{
    vec3 albedo = texture2D( carbonColor, DataIn.uv ).rgb;
    albedo = mix( vec3(1.0), albedo, 0.5 );

    vec3 l = lightPosition - DataIn.position;
    float d = length(l);
    l /= d;

    mat3 tbn;
    tbn[0] = normalize( -DataIn.tangent );
    tbn[1] = normalize( DataIn.binormal );
    tbn[2] = normalize( DataIn.normal );
    
    vec3 n = texture2D( carbonNormal, DataIn.uv ).rgb;
    n.xy = 2.0 * n.xy - 1.0;
    n.z = sqrt( 1.0 - dot(n.xy,n.xy) );
    n = normalize(n);
    n = tbn * n;

    vec3 diff = mix( lightColor.rgb, lightColor.bgr, 0.5 + 0.5 * sin( time ) ) * vec3(max( dot( l, n ), 0.0 ));

    float att = lightIntensity * max( 1.0 - d * d / ( lightRadius * lightRadius ), 0.0 );

    vec3 ambientColor = mix( gndColor, skyColor, 0.5 + 0.5 * n.yyy );
    float ambientIntensity = ambient * mix( 1.0, DataIn.color.x, aoIntensity ) * mix( 1.0, 0.5, dot( n, normalize( vec3( 1.0, 1.0, 1.0 ) ) ) );

    outColor = vec4( albedo * ( att * att * diff + ambientIntensity * ambientColor ) , 1.0 );
}
