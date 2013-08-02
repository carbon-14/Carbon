
in vec2 uv;

out float outValue;

layout(binding=0) uniform sampler2D depthStencilTexture;

void main()
{
    float z = texture( depthStencilTexture, uv ).x;
    
    outValue = DepthRange.x / (  DepthRange.y - z * DepthRange.z );
}
