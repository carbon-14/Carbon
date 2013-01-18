#include "Graphic/TextureResource.h"

namespace Graphic
{
    struct TextureHeader
    {
        U32     internalFormat;
        U32     externalFormat;
        U32     mipMapCount;
        Bool    compressed;
    };

    struct LevelDesc
    {
        U32 size;
        U32 width;
        U32 height;
    };

    TextureResource::TextureResource()
        : Core::Resource()
    {
    }

    TextureResource::~TextureResource()
    {
        RenderDevice::DestroyTexture( m_texture );
    }

    Handle TextureResource::GetTexture() const
    {
        return m_texture;
    }

    void TextureResource::Load( const void * data )
    {
        U8 * ptr = (U8*)data;

        TextureHeader * header = (TextureHeader*)ptr;

        ptr += sizeof(TextureHeader);

        const SizeT maxMipMapCount = 16;
        SizeT sizes[ maxMipMapCount ];
        SizeT widths[ maxMipMapCount ];
        SizeT heights[ maxMipMapCount ];
        void * datas[ maxMipMapCount ];

        SizeT count = ( header->mipMapCount < maxMipMapCount ) ? header->mipMapCount : maxMipMapCount;
        for ( SizeT i=0; i<count; ++i )
        {
            LevelDesc * desc = (LevelDesc*)ptr;
            ptr += sizeof(LevelDesc);

            sizes[ i ]      = desc->size;
            widths[ i ]     = desc->width;
            heights[ i ]    = desc->height;
            datas[ i ]      = ptr;
            ptr            += desc->size;
        }

        m_texture = RenderDevice::CreateTexture( header->internalFormat, header->externalFormat, count, header->compressed, sizes, widths, heights, datas );

        m_state = m_texture ? LOADED : INVALID;
    }
}
