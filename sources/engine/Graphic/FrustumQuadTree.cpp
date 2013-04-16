#include "Graphic/FrustumQuadTree.h"

#include "Graphic/Camera.h"

#include "Core/MemoryUtils.h"
#include "Core/MemoryManager.h"
#include "Core/Math.h"

namespace Graphic
{
    FrustumQuadTree::FrustumQuadTree()
        : m_tileSize( 0 )
        , m_width( 0 )
        , m_height( 0 )
        , m_camera( 0 )
        , m_levelCount( 0 )
        , m_cells( 0 )
        , m_resultCount( 0 )
        , m_results( 0 )
    {
    }

    FrustumQuadTree::~FrustumQuadTree()
    {
        Clear();
    }

    void FrustumQuadTree::Build( SizeT tileSize, SizeT width, SizeT height, const Camera * camera )
    {
        CARBON_ASSERT( m_cells == 0 );
        CARBON_ASSERT( tileSize > 0 );
        CARBON_ASSERT( width > 0 );
        CARBON_ASSERT( height > 0 );
        CARBON_ASSERT( camera );

        m_tileSize  = tileSize;
        m_width     = width;
        m_height    = height;
        m_camera    = camera;

        SizeT tiles = IMax( m_width, m_height ) / m_tileSize;
        m_levelCount = 0;
        while ( ( tiles >> m_levelCount ) != 0 ) { ++m_levelCount; }

        CARBON_ASSERT( m_levelCount > 0 );
        CARBON_ASSERT( m_levelCount < 16 );

        SizeT count = ( 1 << ( 2 * m_levelCount ) ) / 3;
        m_cells = static_cast< Cell * >( MemoryManager::Malloc( sizeof(Cell) * count, MemoryUtils::AlignOf<Cell>() ) );

        SizeT sizeMax = m_tileSize * ( 1 << m_levelCount );

        if ( m_camera->m_projectionType == PT_ORTHOGRAPHIC )
        {
            Vector screenSize = Splat( (F32)sizeMax ) * Vector2( m_camera->m_width / m_width, camera->m_height / m_height );
            Vector pos = Splat( -0.5f ) * screenSize;
            pos = Select( pos, UnitW, Mask<0,0,1,1>() );

            Cell * ptr = m_cells;
            for ( SizeT k=0; k<m_levelCount; ++k )
            {
                SizeT size = 1 << k;

                Vector scale = screenSize / Splat( (F32)size );
                Vector offset = pos + Splat( 0.5f ) * scale;

                for ( SizeT i=0; i<size; ++i )
                {
                    for ( SizeT j=0; j<size; ++j )
                    {
                        Vector center = offset + scale * Vector2( (F32)i, (F32)j );

                        ptr->id = size * i + j;
                        ptr->hPlane = Swizzle<2,3,2,1>( center );
                        ptr->vPlane = Swizzle<3,2,2,0>( center );
                        ++ptr;
                    }
                }
            }
        }
        else
        {
            F32 tanFOV = sizeMax * Tan( 0.5f * m_camera->m_fov );

            Vector screenSize = Vector2( tanFOV / m_width, tanFOV / ( m_height * m_camera->m_aspectRatio ) );
            Vector pos = Splat( -0.5f ) * screenSize;
            pos = Select( pos, Vector3( 0.0f, 0.0f, -1.0f ), Mask<0,0,1,1>() );

            Cell * ptr = m_cells;
            for ( SizeT k=0; k<m_levelCount; ++k )
            {
                SizeT size = 1 << k;

                Vector scale = screenSize / Splat( (F32)size );
                Vector offset = pos + Splat( 0.5f ) * scale;

                for ( SizeT i=0; i<size; ++i )
                {
                    for ( SizeT j=0; j<size; ++j )
                    {
                        Vector dir = offset +  scale * Vector2( (F32)i, (F32)j );

                        ptr->id = size * i + j;
                        ptr->hPlane = Normalize( Cross( UnitX, dir ) );
                        ptr->vPlane = Normalize( Cross( dir, UnitY ) );
                        ++ptr;
                    }
                }
            }
        }        
    }

    void FrustumQuadTree::Clear()
    {
        MemoryManager::Free( m_cells );
        m_tileSize  = 0;
        m_width     = 0;
        m_height    = 0;
        m_camera    = 0;
        m_cells     = 0;
        m_levelCount = 0;
    }

    void FrustumQuadTree::Begin()
    {

    }

    void FrustumQuadTree::End()
    {

    }

    SizeT FrustumQuadTree::GetTileSize() const
    {
        return m_tileSize;
    }

    SizeT FrustumQuadTree::GetWidth() const
    {
        return m_width;
    }
    
    SizeT FrustumQuadTree::GetHeight() const
    {
        return m_height;
    }

    const Camera * FrustumQuadTree::GetCamera() const
    {
        return m_camera;
    }

    SizeT FrustumQuadTree::GetLevelCount() const
    {
        return m_levelCount;
    }

    const FrustumQuadTree::Cell * FrustumQuadTree::GetCells() const
    {
        return m_cells;
    }
}
