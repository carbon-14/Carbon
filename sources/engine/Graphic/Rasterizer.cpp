#include "Graphic/Rasterizer.h"

#include "Graphic/RenderDevice.h"
#include "Graphic/Camera.h"

#include "Core/MemoryManager.h"
#include "Core/Math.h"

namespace Graphic
{
    Rasterizer::Rasterizer()
    {

    }

    Rasterizer::~Rasterizer()
    {

    }

    void Rasterizer::Initialize()
    {
        const U32 programTileDepthId    = ProgramCache::CreateId( "tileDepth" );
        m_programTileDepth              = ProgramCache::GetProgram( programTileDepthId );
    }

    void Rasterizer::Destroy()
    {

    }

    Rasterizer::Context * Rasterizer::CreateContext()
    {
        Context * context = MemoryManager::New< Context >();

        context->m_camera = 0;
        context->m_linearDepthTexture = 0;
        context->m_width = 0;
        context->m_height = 0;
        context->m_size = 0;
        
        context->m_depthBuffer = 0;
        context->m_mapBuffer = 0;
        context->m_mapInfos = 0;

        return context;
    }

    void Rasterizer::UpdateContext( Context * context, SizeT width, SizeT height, const Camera * camera, Handle linearDepthTexture )
    {
        context->m_camera = camera;
        context->m_linearDepthTexture = linearDepthTexture;

        context->m_width = width / ms_tileSize;
        context->m_height = height / ms_tileSize;
        context->m_size = context->m_width * context->m_height;

        //context->m_vCount = width / ms_tileSize + 1;
        //context->m_hCount = height / ms_tileSize + 1;

        if ( camera->m_projectionType == PT_PERSPECTIVE )
        {
            context->m_vPlanes.Resize( context->m_width + 1 );
            context->m_hPlanes.Resize( context->m_height + 1 );
            /*if ( context->m_vCount > context->m_vSize )
            {
                MemoryManager::Free( context->m_vPlanes );
                context->m_vPlanes = reinterpret_cast< Vector * >( MemoryManager::Malloc( sizeof(Vector) * context->m_vCount, MemoryUtils::AlignOf< Vector >() ) );
                context->m_vSize = context->m_vCount;
            }
            if ( context->m_hCount > context->m_hSize )
            {
                MemoryManager::Free( context->m_hPlanes );
                context->m_hPlanes = reinterpret_cast< Vector * >( MemoryManager::Malloc( sizeof(Vector) * context->m_hCount, MemoryUtils::AlignOf< Vector >() ) );
                context->m_hSize = context->m_hCount;
            }*/

            F32 size = 2.0f * Tan( 0.5f * camera->m_fov );

            Vector vScale = Splat( size ) * camera->GetInvViewMatrix().m_column[0];

            for ( SizeT i=0; i<context->m_vPlanes.Size(); ++i )
            {
                Vector dir = vScale * Splat( Min( static_cast< F32 >( i * ms_tileSize ) / width, 1.0f ) - 0.5f ) - camera->GetInvViewMatrix().m_column[2];
                context->m_vPlanes[ i ] = Normalize( Cross( dir, camera->GetInvViewMatrix().m_column[1] ) );
                context->m_vPlanes[ i ] = Select( context->m_vPlanes[ i ], Dot( context->m_vPlanes[ i ], camera->m_position ), Mask<0,0,0,1>() );
            }

            Vector hScale = Splat( size / camera->m_aspectRatio ) * camera->GetInvViewMatrix().m_column[1];
            for ( SizeT i=0; i<context->m_hPlanes.Size(); ++i )
            {
                Vector dir = hScale * Splat( Min( static_cast< F32 >( i * ms_tileSize ) / height, 1.0f ) - 0.5f ) - camera->GetInvViewMatrix().m_column[2];
                context->m_hPlanes[ i ] = Normalize( Cross( camera->GetInvViewMatrix().m_column[0], dir ) );
                context->m_hPlanes[ i ] = Select( context->m_hPlanes[ i ], Dot( context->m_hPlanes[ i ], camera->m_position ), Mask<0,0,0,1>() );
            }
        }

        if ( context->m_size > context->m_mapCount.Capacity() )
        {
            RenderDevice::DestroyTexture( context->m_mapInfos );
            RenderDevice::DestroyTexture( context->m_mapBuffer );
            RenderDevice::DestroyTexture( context->m_depthBuffer );

            context->m_depthBuffer = RenderDevice::CreateShaderStorageBuffer( context->m_size * sizeof(U32), 0, BU_DYNAMIC );
            context->m_mapBuffer = RenderDevice::CreateShaderStorageBuffer( context->m_size * sizeof(U16) * 8, 0, BU_DYNAMIC );
            context->m_mapInfos = RenderDevice::CreateShaderStorageBuffer( context->m_size * sizeof(U32), 0, BU_DYNAMIC );
        }

        context->m_mapCount.Resize( context->m_size );
        context->m_mapData.Resize( context->m_size * ms_maxCount );

        MemoryUtils::MemSet( context->m_mapCount.Ptr(), 0, context->m_size );
    }

    void Rasterizer::DestroyContext( Context * context )
    {
        RenderDevice::DestroyTexture( context->m_mapInfos );
        RenderDevice::DestroyTexture( context->m_mapBuffer );
        RenderDevice::DestroyTexture( context->m_depthBuffer );

        MemoryManager::Delete( context );
    }

    void Rasterizer::Draw( const Context * context ) const
    {
        RenderDevice::BindImageTexture( context->m_linearDepthTexture, 0, 0, BA_READ_ONLY, TF_R32F );
        RenderDevice::BindShaderStorageBuffer( context->m_depthBuffer, 0 );
        ProgramCache::UseProgram( m_programTileDepth );
        RenderDevice::DispatchCompute( context->m_width, context->m_height, 1 );
        glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );

        CARBON_ASSERT( context->m_size < 128*128 );

        U16 depth[128*128*2];
        MemoryUtils::MemCpy( depth, RenderDevice::MapShaderStorageBuffer( context->m_depthBuffer, BA_READ_ONLY ), context->m_size * sizeof(U32) );
        RenderDevice::UnmapShaderStorageBuffer();

        Vector nearPlane = context->m_camera->GetFrustum().GetPlanes()[ FP_NEAR ];

        Vector nearDepth = Splat( context->m_camera->m_near );
        Vector depthRange = Splat( ( context->m_camera->m_far - context->m_camera->m_near ) / 65535.0f );

        U16 buffer[128*128*8];
        U16 infos[128*128][2];

        U16 dstCount = 0;
        for ( SizeT i=0; i<context->m_size; ++i )
        {
            Vector maxDepth = nearDepth + depthRange * Splat( static_cast< F32 >( depth[ 2 * i ] ) );
            Vector minDepth = nearDepth + depthRange * Splat( static_cast< F32 >( depth[ 2 * i + 1 ] ) );

            U16 offset = dstCount;

            U8 srcCount = context->m_mapCount[i];
            
            U8 r = srcCount % 4;
            U8 q = srcCount - r;
            const Vector * const * it = context->m_mapData.ConstPtr() + i * ms_maxCount;
            const Vector * const * end = it + q;
            while ( it != end )
            {
                const Vector * s0 = *(it++);
                const Vector * s1 = *(it++);
                const Vector * s2 = *(it++);
                const Vector * s3 = *(it++);

                Matrix m = { *s0, *s1, *s2, *s3 };

                F128 results;
                Store( results, Cull4( m, nearPlane, minDepth, maxDepth ) );

                if ( results[0] ){ buffer[dstCount++] = s0 - context->m_spheres.ConstPtr(); }
                if ( results[1] ){ buffer[dstCount++] = s1 - context->m_spheres.ConstPtr(); }
                if ( results[2] ){ buffer[dstCount++] = s2 - context->m_spheres.ConstPtr(); }
                if ( results[3] ){ buffer[dstCount++] = s3 - context->m_spheres.ConstPtr(); }
            }

            if ( r )
            {
                Matrix m;
                m.m_column[0] = *it[0];
                m.m_column[1] = ( r > 1 ) ? *it[1] : Zero4;
                m.m_column[2] = ( r > 2 ) ? *it[2] : Zero4;
                m.m_column[3] = Zero4;

                F128 results;
                Store( results, Cull4( m, nearPlane, minDepth, maxDepth ) );

                if ( results[0] )			{ buffer[dstCount++] = it[0] - context->m_spheres.ConstPtr(); }
                if ( results[1] && r > 1 )	{ buffer[dstCount++] = it[1] - context->m_spheres.ConstPtr(); }
                if ( results[2] && r > 2 )	{ buffer[dstCount++] = it[2] - context->m_spheres.ConstPtr(); }
            }

            infos[i][0] = offset;
            infos[i][1] = dstCount;
        }

        MemoryUtils::MemCpy( RenderDevice::MapShaderStorageBuffer( context->m_mapBuffer, BA_WRITE_ONLY ), buffer, dstCount * sizeof(U16) );
        RenderDevice::UnmapShaderStorageBuffer();

        MemoryUtils::MemCpy( RenderDevice::MapShaderStorageBuffer( context->m_mapInfos, BA_WRITE_ONLY ), infos, context->m_size * sizeof(U32) );
        RenderDevice::UnmapShaderStorageBuffer();
    }

    void Rasterizer::FillQuad( const Vector ** quad, const Vector * sphere, Context * context ) const
    {
        SizeT rowBegin = quad[0] - context->m_vPlanes.Begin();
        SizeT rowEnd = quad[1] - context->m_vPlanes.Begin();
        SizeT colBegin = quad[2] - context->m_hPlanes.Begin();
        SizeT colEnd = quad[3] - context->m_hPlanes.Begin();

        SizeT pitchCount = context->m_vPlanes.Capacity() - 1;
        SizeT pitchMap = ( context->m_vPlanes.Capacity() - 1 ) * ms_maxCount;

        const Vector ** colMap = context->m_mapData.Begin() + colBegin * pitchMap;
        U8 * colCount = context->m_mapCount.Begin() + colBegin * pitchCount;
        U8 * colCountEnd = context->m_mapCount.Begin() + colEnd * pitchCount;
        for ( ; colCount!=colCountEnd; colCount += pitchCount, colMap += pitchMap )
        {
            const Vector ** map = colMap + rowBegin * ms_maxCount;
            U8 * count = colCount + rowBegin;
            U8 * countEnd = colCount + rowEnd;
            for ( ; count!=countEnd; ++count, map += ms_maxCount )
            {
                CARBON_ASSERT( *count < ms_maxCount );
                map[ (*count)++ ] = sphere;
            }
        }
    }

    Vector Rasterizer::Cull4( const Matrix& m, const Vector& nearPlane, const Vector& minDepth, const Vector& maxDepth ) const
    {
        Matrix p = Transpose( m );

        Vector radius = p.m_column[3];
        p.m_column[3] = One4;

        Vector depth = Mul( p, nearPlane );

        Vector front = ( depth + radius ) > minDepth;
        Vector back = ( depth - radius ) < maxDepth;

        return And( front, back );
    }
}
