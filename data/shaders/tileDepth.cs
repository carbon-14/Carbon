
layout (local_size_x = 16, local_size_y = 16) in;

layout(binding = 0, r32f) uniform image2D depthImage;

layout(binding = 0) writeonly buffer tiledDepth
{
    uint tiledDepthBuffer[];
};

shared uint minDepth;
shared uint maxDepth;

void main()
{
    uint depth = uint( 65535.0 * imageLoad(depthImage, ivec2(gl_GlobalInvocationID.xy)) );
    
    if ( gl_LocalInvocationIndex == 0 )
    {
        minDepth = 0xffff;
        maxDepth = 0x0000;
    }
    
    barrier();

    atomicMin( minDepth, depth );
    atomicMax( maxDepth, depth );

    barrier();

    if ( gl_LocalInvocationIndex == 0 )
    {
        uint id = gl_WorkGroupID.x + gl_NumWorkGroups.x * gl_WorkGroupID.y;

        tiledDepthBuffer[ id ] = ( maxDepth << 16 ) | minDepth;
    }
}
