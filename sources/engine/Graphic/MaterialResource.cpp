#include "Graphic/MaterialResource.h"

#include "Graphic/TextureResource.h"

namespace Graphic
{
    MaterialResource::MaterialResource()
        : Core::Resource()
        , m_program( ProgramCache::ms_invalidHandle )
        , m_textureCount( 0 )
    {
    }

    MaterialResource::~MaterialResource()
    {
    }

    ProgramHandle MaterialResource::GetProgram() const
    {
        return m_program;
    }

    const TextureResource * MaterialResource::GetTexture( SizeT index ) const
    {
        CARBON_ASSERT( index < RenderDevice::ms_maxTextureUnitCount );
        return m_textures[ index ].ConstPtr();
    }

    SizeT MaterialResource::GetTextureCount() const
    {
        return m_textureCount;
    }

    void MaterialResource::Load( const void * data )
    {
    }
}
