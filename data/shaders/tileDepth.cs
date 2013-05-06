
layout (local_size_x = 16, local_size_y = 16) in;

layout(binding = 0, r32f) uniform image2D linearDepthImg;

layout(binding = 0) writeonly buffer depthBuffer
{
    uint depth[];
};

shared int minDepth;
shared int maxDepth;

void main()
{
    float linearDepth = imageLoad(linearDepthImg, ivec2(gl_GlobalInvocationID.xy));
    
    if ( gl_LocalInvocationIndex == 0 )
    {
        minDepth = 0xffff;
        maxDepth = 0x0000;
    }
    
    barrier();

    if ( linearDepth != 0.0 )
    {
        int iLinearDepth = int( 65535.0 * linearDepth );

        atomicMin( minDepth, iLinearDepth );
        atomicMax( maxDepth, iLinearDepth );
    }

    barrier();

    if ( gl_LocalInvocationIndex == 0 )
    {
        uint id = gl_WorkGroupID.x + gl_NumWorkGroups.x * gl_WorkGroupID.y;

        depth[ id ] = ( minDepth << 16 ) | maxDepth;
    }
}
