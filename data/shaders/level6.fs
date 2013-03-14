
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

layout(binding=0) uniform sampler2D colorMap;
layout(binding=1) uniform sampler2D normalMap;

layout(binding=0) uniform CameraParameters
{
    mat4    viewProjMat;
    vec4    camPosition;
};

layout(binding=1) uniform AmbientParameters
{
    vec4    groundColor;
    vec4    skyColor;
};

layout(binding=2) uniform LightParameters
{
    vec4    lightPos;
    vec4    lightDir;
    vec4    lightColor;
    float   lightRadius;
};

layout(binding=3) uniform FlashParameters
{
    vec4    flashPos;
    vec4    flashDir;
    vec4    flashColor;
    float   flashRadius;
};

layout(binding=UNIFORM_BINDING_MATERIAL) uniform MaterialParameters
{
    vec4    SpecParams;
};

void main()
{
    mat3 tbn = mat3(    normalize( DataIn.tangent ),
                        normalize( DataIn.binormal ),
                        normalize( DataIn.normal ) );

    vec4 color = texture( colorMap, DataIn.uv );

    vec3 albedo = color.rgb;

    vec3 n  = texture( normalMap, DataIn.uv ).rgb;
    n.xy    = 2.0 * n.xy - 1.0;
    n.z     = sqrt( 1.0 - dot( n.xy, n.xy ) );
    n       = tbn * n;

    vec3 v = normalize( camPosition.xyz - DataIn.position );

    vec3 light = vec3(0.0);
    {
        vec3 l = lightPos.xyz - DataIn.position;
        float d = length(l);
        l /= d;

        float att = max( 1.0 - d * d / ( lightRadius * lightRadius ), 0.0 );

        float diff  = max( dot( l, n ), 0.0 );
        float spec  = SpecParams.x * pow( max( dot( normalize( l + v ), n ), 0.0 ), SpecParams.y );

        light = att * att * ( diff + spec ) *  lightColor.rgb;
    }

    vec3 flash = vec3(0.0);
    {
        vec3 l = flashPos.xyz - DataIn.position;
        float d = length(l);
        l /= d;

        float att = max( 1.0 - d * d / ( flashRadius * flashRadius ), 0.0 );
        att *= ( clamp( dot( l, -flashDir.xyz ), 0.85, 1.0 ) - 0.85 ) / 0.15;

        float diff  = max( dot( l, n ), 0.0 );
        float spec  = SpecParams.x * pow( max( dot( normalize( l + v ), n ), 0.0 ), SpecParams.y );

        flash = att * att * ( diff + spec ) * flashColor.rgb;
    }

    vec3 ambient = vec3(0.0);
    {
        vec3 ambientColor = mix( groundColor.rgb * groundColor.a, skyColor.rgb * skyColor.a, 0.5 + 0.5 * n.yyy );
        float ambientIntensity = DataIn.color.x * mix( 1.0, 0.5, dot( n, normalize( vec3( 1.0, 1.0, 1.0 ) ) ) );
        ambient = ambientIntensity * ambientColor;
    }

    outColor = vec4( albedo * ( light + flash + ambient ), 1.0 );
}
