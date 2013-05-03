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

    }

    void Rasterizer::Destroy()
    {

    }

    Rasterizer::Context * Rasterizer::CreateContext()
    {
        Context * context = MemoryManager::New< Context >();

        context->m_camera = 0;
        context->m_vPlanes = 0;
        context->m_hPlanes = 0;
        context->m_vSize = 0;
        context->m_vCount = 0;
        context->m_hSize = 0;
        context->m_hCount = 0;
        context->m_depthMinMaxTexture = 0;

        return context;
    }

    void Rasterizer::UpdateContext( Context * context, SizeT width, SizeT height, const Camera * camera )
    {
        context->m_camera = camera;

        if ( camera->m_projectionType == PT_PERSPECTIVE )
        {
            context->m_vCount = width / ms_tileSize + ( ( width % ms_tileSize ) ? 2 : 1 );
            context->m_hCount = height / ms_tileSize + ( ( height % ms_tileSize ) ? 2 : 1 );

            if ( context->m_vCount > context->m_vSize )
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
            }

            F32 size = 2.0f * Tan( 0.5f * camera->m_fov );

            Vector vScale = Splat( size ) * camera->GetInvViewMatrix().m_column[0];
            for ( SizeT i=0; i<context->m_vCount; ++i )
            {
                Vector dir = vScale * Splat( Min( static_cast< F32 >( i * ms_tileSize ) / width, 1.0f ) - 0.5f ) - camera->GetInvViewMatrix().m_column[2];
                context->m_vPlanes[ i ] = Normalize( Cross( dir, camera->GetInvViewMatrix().m_column[1] ) );
                context->m_vPlanes[ i ] = Select( context->m_vPlanes[ i ], Dot( context->m_vPlanes[ i ], camera->m_position ), Mask<0,0,0,1>() );
            }

            Vector hScale = Splat( size / camera->m_aspectRatio ) * camera->GetInvViewMatrix().m_column[1];
            for ( SizeT i=0; i<context->m_hCount; ++i )
            {
                Vector dir = hScale * Splat( Min( static_cast< F32 >( i * ms_tileSize ) / height, 1.0f ) - 0.5f ) - camera->GetInvViewMatrix().m_column[2];
                context->m_hPlanes[ i ] = Normalize( Cross( camera->GetInvViewMatrix().m_column[0], dir ) );
                context->m_hPlanes[ i ] = Select( context->m_hPlanes[ i ], Dot( context->m_hPlanes[ i ], camera->m_position ), Mask<0,0,0,1>() );
            }
        }
    }

    void Rasterizer::DestroyContext( Context * context )
    {
        RenderDevice::DestroyTexture( context->m_depthMinMaxTexture );

        context->m_hCount = 0;
        context->m_vCount = 0;

        MemoryManager::Free( context->m_hPlanes );
        MemoryManager::Free( context->m_vPlanes );

        MemoryManager::Delete( context );
    }

    void Rasterizer::Render( const Vector * const * spheres, SizeT sphereCount, Context * context ) const
    {

    }

    void Rasterizer::Draw( const Context ) const
    {

    }
}
