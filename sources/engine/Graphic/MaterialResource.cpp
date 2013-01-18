#include "Graphic/MaterialResource.h"

#include "Graphic/TextureResource.h"

#include "Core/ResourceManager.h"

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

    const MaterialResource::Texture& MaterialResource::GetTexture( SizeT index ) const
    {
        CARBON_ASSERT( index < RenderDevice::ms_maxTextureUnitCount );
        return m_textures[ index ];
    }

    SizeT MaterialResource::GetTextureCount() const
    {
        return m_textureCount;
    }

    void MaterialResource::Load( const void * data )
    {
        const U8 * ptr = (U8*)data;

        U32 programId = *((U32*)ptr);
        ptr += sizeof(U32);

        U32 setId = *((U32*)ptr);
        ptr += sizeof(U32);

        m_program = ProgramCache::GetProgram( programId, setId );

        U32 texCount = *((U32*)ptr);
        ptr += sizeof(U32);

        for ( m_textureCount=0; m_textureCount<texCount; ++m_textureCount )
        {
            Texture& tex = m_textures[m_textureCount];

            tex.m_index = *((U32*)ptr);
            ptr += sizeof(U32);

            tex.m_resource = Core::ResourceManager::Create< TextureResource >( (const Char*)ptr );
            ptr += 256;
        }
    }
}
