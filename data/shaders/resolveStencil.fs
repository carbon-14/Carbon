
in vec2 uv;

out uint outValue;

layout(binding=0) uniform usampler2D depthStencilTexture;

void main()
{
    outValue = texture( depthStencilTexture, uv ).x;
}
