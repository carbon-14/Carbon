
in vec2 uv;

out vec4 outColor;

layout(binding=0) uniform sampler2D LightTexture;

void main()
{
    outColor = texture2D( LightTexture, uv );
}
