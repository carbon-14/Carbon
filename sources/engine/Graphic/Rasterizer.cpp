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

        context->m_vPlanes = 0;
        context->m_hPlanes = 0;
        context->m_near = 0;
        context->m_far = 0;
        context->m_vSize = 0;
        context->m_vCount = 0;
        context->m_hSize = 0;
        context->m_hCount = 0;
        context->m_depthMinMaxTexture = 0;

        return context;
    }

    void Rasterizer::UpdateContext( Context * context, SizeT width, SizeT height, const Camera * camera )
    {
        context->m_vCount = width / ms_tileSize + ( ( width % ms_tileSize ) ? 2 : 1 );
        context->m_hCount = height / ms_tileSize + ( ( height % ms_tileSize ) ? 2 : 1 );

        context->m_near = camera->m_near;
        context->m_far = camera->m_far;

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

        if ( camera->m_projectionType == PT_ORTHOGRAPHIC )
        {
            F32 vOffset = -0.5f * width;
            for ( SizeT i=0; i<context->m_vCount; ++i )
            {
                F32 d = vOffset + Min( static_cast< F32 >( i * ms_tileSize ) / width, 1.0f );
                context->m_vPlanes[ i ] = Vector4( 1.0f, 0.0f, 0.0f, d );
            }

            F32 hOffset = -0.5f * height;
            for ( SizeT i=0; i<context->m_hCount; ++i )
            {
                F32 d = hOffset + Min( static_cast< F32 >( i * ms_tileSize ) / height, 1.0f );
                context->m_hPlanes[ i ] = Vector4( 0.0f, 1.0f, 0.0f, d );
            }
        }
        else
        {
            F32 size = 2.0f * Tan( 0.5f * camera->m_fov );

            F32 vScale = size;
            for ( SizeT i=0; i<context->m_vCount; ++i )
            {
                F32 w = vScale * ( Min( static_cast< F32 >( i * ms_tileSize ) / width, 1.0f ) - 0.5f );
                context->m_vPlanes[ i ] = Normalize( Cross( Vector3( w, 0.0f, -1.0f ), Vector3( 0.0f, 1.0f, 0.0f )  ) );
            }

            F32 hScale = size / camera->m_aspectRatio;
            for ( SizeT i=0; i<context->m_hCount; ++i )
            {
                F32 h = hScale * ( Min( static_cast< F32 >( i * ms_tileSize ) / height, 1.0f ) - 0.5f );
                context->m_hPlanes[ i ] = Normalize( Cross( Vector3( 1.0f, 0.0f, 0.0f ), Vector3( 0.0f, h, -1.0f ) ) );
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
