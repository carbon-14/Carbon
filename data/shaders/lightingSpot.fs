
out vec4 outColor;

DECLARE_LIGHT_PARAMETERS;

layout(binding=0) uniform sampler2D GBufferDepth;
layout(binding=1) uniform sampler2D GBufferNormal;
layout(binding=2) uniform sampler2D GBufferColor;

void main()
{
    vec2 uv = gl_FragCoord.xy * viewportSize.zw;

    GBuffer gbuffer = ReadGBuffer( GBufferDepth, GBufferNormal, GBufferColor, uv );

    vec3 pos = vec3( 2.0 * uv - 1.0, 1.0 ) * viewScale.xyz * gbuffer.depth;

    vec3 L = LightPosition.xyz - pos;

    float sqr = dot( L, L );
    float att = 1.0 - min( sqr * LightInvSqrRadius, 1.0 );

    L /= sqrt( sqr );

    float k = dot( LightDirection.xyz, -L );
    
    const float cos_angle = sqrt( 2.0f ) / 2.0;

    att *=  ( clamp( k, cos_angle, 1.0 ) - cos_angle ) /  ( 1.0 - cos_angle );
    
    att *= att;

    float diffuse_term = max( dot( gbuffer.normal, L ), 0.0 );

    vec3 l = LightValue * att * diffuse_term;

    outColor = vec4( gbuffer.albedo * l, 1.0 );
}
