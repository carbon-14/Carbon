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
            RenderDevice::DestroyBuffer( context->m_depthBuffer );

            context->m_depthBuffer = RenderDevice::CreateShaderStorageBuffer( context->m_size * sizeof(U32), 0, BU_DYNAMIC );
        }

        context->m_mapCount.Resize( context->m_size );
        context->m_map.Resize( context->m_size * ms_maxCount );

        MemoryUtils::MemSet( context->m_mapCount.Ptr(), 0, context->m_size );
    }

    void Rasterizer::DestroyContext( Context * context )
    {
        RenderDevice::DestroyTexture( context->m_depthBuffer );

        MemoryManager::Delete( context );
    }

    void Rasterizer::Render( const Vector * const * spheres, SizeT sphereCount, Context * context ) const
    {
        const Vector * const * it = spheres;
        const Vector * const * end = spheres + sphereCount;
        for ( ; it != end; ++it )
        {
            const Vector * s = *it;

            Vector v = *s;
            Vector c = Select( v, One4, Mask<0,0,0,1>() );
            Vector r = Swizzle<3,3,3,3>( v );

            const Vector * quad[4] =
            {
                context->m_vPlanes.Begin(),
                context->m_vPlanes.End() - 1,
                context->m_hPlanes.Begin(),
                context->m_hPlanes.End() - 1
            };

            Matrix clip = { *quad[0], -*quad[1], *quad[2], -*quad[3] };
            clip = Transpose( clip );

            Vector test = Mul( clip, c ) > r;

            F128 inside;
            Store( inside, test );

            r = r * Vector4( 1.0f, -1.0f, 1.0f, -1.0f );

            if ( inside[0] )
            {
                const Vector * minPlane = quad[0];
                const Vector * maxPlane = quad[1];
                SizeT dist = maxPlane - minPlane;
                while ( dist > 1 )
                {
                    quad[0] = minPlane + dist / 2;

                    test = Dot( c, *quad[0] ) < r;

                    F128 result;
                    Store( result, test );

                    if ( result[0] )    { maxPlane = quad[0]; }
                    else                { minPlane = quad[0]; }

                    dist = maxPlane - minPlane;
                }
            }

            if ( inside[1] )
            {
                const Vector * minPlane = quad[0];
                const Vector * maxPlane = quad[1];
                SizeT dist = maxPlane - minPlane;
                while ( dist > 1 )
                {
                    quad[1] = minPlane + dist / 2;

                    test = Dot( c, *quad[1] ) < r;
                    
                    F128 result;
                    Store( result, test );

                    if ( result[1] )    { maxPlane = quad[1]; }
                    else                { minPlane = quad[1]; }

                    dist = maxPlane - minPlane;
                }
            }

            if ( inside[2] )
            {
                const Vector * minPlane = quad[2];
                const Vector * maxPlane = quad[3];
                SizeT dist = maxPlane - minPlane;
                while ( dist > 1 )
                {
                    quad[2] = minPlane + dist / 2;

                    test = Dot( c, *quad[2] ) < r;
                    
                    F128 result;
                    Store( result, test );

                    if ( result[2] )    { maxPlane = quad[2]; }
                    else                { minPlane = quad[2]; }

                    dist = maxPlane - minPlane;
                }
            }

            if ( inside[3] )
            {
                const Vector * minPlane = quad[2];
                const Vector * maxPlane = quad[3];
                SizeT dist = maxPlane - minPlane;
                while ( dist > 1 )
                {
                    quad[3] = minPlane + dist / 2;

                    test = Dot( c, *quad[3] ) < r;
                    
                    F128 result;
                    Store( result, test );

                    if ( result[3] )    { maxPlane = quad[3]; }
                    else                { minPlane = quad[3]; }

                    dist = maxPlane - minPlane;
                }
            }

            FillQuad( quad, s, context );
        }
    }

    void Rasterizer::Draw( const Context * context ) const
    {
        RenderDevice::BindImageTexture( context->m_linearDepthTexture, 0, 0, BA_READ_ONLY, TF_R32F );
        RenderDevice::BindShaderStorageBuffer( context->m_depthBuffer, 0 );
        ProgramCache::UseProgram( m_programTileDepth );
        RenderDevice::DispatchCompute( context->m_vPlanes.Size(), context->m_hPlanes.Size(), 1 );
        glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );

        CARBON_ASSERT( context->m_size < 128*128 );

        U16 depth[128*128*2];
        MemoryUtils::MemCpy( depth, RenderDevice::MapShaderStorageBuffer( context->m_depthBuffer, BA_READ_ONLY ), context->m_size * sizeof(U32) );
        RenderDevice::UnmapShaderStorageBuffer();

        for ( SizeT i=0; i<context->m_size; ++i )
        {
            U8 count = context->m_mapCount[i];
            U16 minDepth = depth[ 2 * i ];
            U16 maxDepth = depth[ 2 * i + 1 ];
            for ( SizeT j=0; j<count; ++j )
            {
                const Vector * s = context->m_map[ i * ms_maxCount + j ];



            }
        }
    }

    void Rasterizer::FillQuad( const Vector ** quad, const Vector * sphere, Context * context ) const
    {
        SizeT rowBegin = quad[0] - context->m_vPlanes.Begin();
        SizeT rowEnd = quad[1] - context->m_vPlanes.Begin() + 1;
        SizeT colBegin = quad[2] - context->m_hPlanes.Begin();
        SizeT colEnd = quad[3] - context->m_hPlanes.Begin() + 1;

        SizeT pitchCount = context->m_hPlanes.Capacity() - 1;
        SizeT pitchMap = ( context->m_hPlanes.Capacity() - 1 ) * ms_maxCount;

        U8 * colCount = context->m_mapCount.Begin() + colBegin * pitchCount;
        const Vector ** colMap = context->m_map.Begin() + colBegin * pitchMap;
        U8 * colCountEnd = context->m_mapCount.Begin() + colEnd * pitchCount;
        for ( ; colCount!=colCountEnd; colCount += pitchCount, colMap += pitchMap )
        {
            U8 * count = colCount + rowBegin;
            const Vector ** map = colMap + rowBegin * ms_maxCount;
            U8 * countEnd = colCount + rowEnd;
            for ( ; count!=countEnd; ++count, map += ms_maxCount )
            {
                CARBON_ASSERT( *count < ms_maxCount );
                map[ (*count)++ ] = sphere;
            }
        }
    }
}
