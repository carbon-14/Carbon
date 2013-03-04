
in vec2 uv;

out float outValue;

layout(binding=0) uniform sampler2D depthTexture;

void main()
{
    float z = texture2D( depthTexture, uv ).x;
    
    outValue = DepthRange.x / (  DepthRange.y - z * DepthRange.z );
}
