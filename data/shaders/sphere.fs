#version 420

in Data
{
    vec3 position;
    vec3 normal;
    vec3 tangent;
    vec3 binormal;
    vec2 uv;
} DataIn;

out vec4 outColor;

layout(binding=0) uniform sampler2D sphereColor;
layout(binding=1) uniform sampler2D sphereNormal;

layout(binding=1) uniform AmbientParameters
{
    vec4    groundColor;
    vec4    skyColor;
};

layout(binding=2) uniform LightParameters
{
    vec4    lightPosition;
    vec4    lightDirection;
    vec4    lightColor;
    float   lightRadius;
};

layout(binding=3) uniform FlashParameters
{
    vec4    flashPosition;
    vec4    flashDirection;
    vec4    flashColor;
    float   flashRadius;
};

const vec3 emissiveColor = 2.0 * vec3( 1.0f, 0.5, 0.25 );

void main()
{
    mat3 tbn = mat3(    normalize( DataIn.tangent ),
                        normalize( DataIn.binormal ),
                        normalize( DataIn.normal ) );

    vec4 color = texture2D( sphereColor, DataIn.uv );

    vec3 albedo = color.rgb;
    vec3 emissive = color.a * emissiveColor;

    vec3 n  = texture2D( sphereNormal, DataIn.uv ).rgb;
    n.xy    = 2.0 * n.xy - 1.0;
    n.z     = sqrt( 1.0 - dot( n.xy, n.xy ) );
    n       = tbn * n;

    vec3 light = vec3(0.0);
    {
        vec3 l = lightPosition.xyz - DataIn.position;
        float d = length(l);
        l /= d;

        vec3 diff = lightColor.a * lightColor.rgb * max( dot( l, n ), 0.0 );
        float att = max( 1.0 - d * d / ( lightRadius * lightRadius ), 0.0 );
        light = att * att * diff;
    }

    vec3 flash = vec3(0.0);
    {
        vec3 l = flashPosition.xyz - DataIn.position;
        float d = length(l);
        l /= d;

        vec3 diff = flashColor.a * flashColor.rgb * max( dot( l, n ), 0.0 );
        float att = max( 1.0 - d * d / ( flashRadius * flashRadius ), 0.0 );
        att *= ( clamp( dot( l, -flashDirection.xyz ), 0.85, 1.0 ) - 0.85 ) / 0.15;
        flash = att * att * diff;
    }

    vec3 ambient = vec3(0.0);
    {
        vec3 ambientColor = mix( groundColor.rgb * groundColor.a, skyColor.rgb * skyColor.a, 0.5 + 0.5 * n.yyy );
        float ambientIntensity = mix( 1.0, 0.5, dot( n, normalize( vec3( 1.0, 1.0, 1.0 ) ) ) );
        ambient = ambientIntensity * ambientColor;
    }

    outColor = vec4( albedo * ( light + flash + ambient ) + emissive, 1.0 );
}
