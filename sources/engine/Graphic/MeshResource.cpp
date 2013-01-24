#include "Graphic/MeshResource.h"

#include "Graphic/MaterialResource.h"

namespace Graphic
{
    enum VType
    {
        VT_UBYTE    = 0,
        VT_SHORT,
        VT_HALF,
        VT_FLOAT,
        VT_NMVECT      // 2_10_10_10
    };

    const DataType toAttribType[] =
    {
        DT_U8,
        DT_S16,
        DT_F16,
        DT_F32,
        DT_S2_10_10_10
    };

    const SizeT toAttribSize[] =
    {
        1, 
        2, 
        2,
        4,
        1   // used to keep a size of 4 in vertex declaration
    };

    const Bool toAttribNormalized[] =
    {
        true,   // color
        false,
        false,
        false,
        true    // normal
    };

    enum IType
    {
        IT_UBYTE   = 0,
        IT_USHORT,
        IT_ULONG
    };

    const DataType toIndexType[] =
    {
        DT_U8,
        DT_U16,
        DT_U32
    };

    const SizeT toIndexSize[] =
    {
        1, 
        2, 
        4
    };

    struct MeshHeader
    {
        IType   indexType;
        U32     subMeshCount;
        U32     inputCount;
        U32     vertexDataSize;
    };

    struct MeshInput
    {
        VertexSemantic  semantic;
        VType           type;
        U32             size;
        U32             offset;
    };

    MeshResource::MeshResource()
        : Core::Resource()
        , m_subMeshCount( 0 )
    {
    }

    MeshResource::~MeshResource()
    {
    }

    PrimitiveType MeshResource::GetPrimitive() const
    {
        return m_primitive;
    }

    Handle MeshResource::GetVertexBuffer() const
    {
        return m_vertexBuffer;
    }

    const VertexDeclaration& MeshResource::GetVertexDecl() const
    {
        return m_vertexDecl;
    }

    SizeT MeshResource::GetVertexCount() const
    {
        return m_vertexCount;
    }

    DataType MeshResource::GetIndexType() const
    {
        return m_indexType;
    }

    const MeshResource::SubMesh * MeshResource::GetSubMeshes() const
    {
        return m_subMeshes;
    }

    SizeT MeshResource::GetSubMeshCount() const
    {
        return m_subMeshCount;
    }

    bool MeshResource::Load( const void * data )
    {
        m_primitive = PT_TRIANGLES;

        const U8 * ptr = (U8*)data;

        MeshHeader * header = (MeshHeader*)ptr;
        ptr += sizeof(MeshHeader);

        m_indexType = toIndexType[ header->indexType ];

        VertexDeclaration& vDecl = m_vertexDecl;
        vDecl.m_count = header->inputCount;
        vDecl.m_size = 0;
        for ( SizeT i=0; i<vDecl.m_count; ++i )
        {
            MeshInput * desc = (MeshInput*)ptr;
            ptr += sizeof(MeshInput);

            vDecl.m_attributes[ i ].m_semantic      = desc->semantic;
            vDecl.m_attributes[ i ].m_type          = toAttribType[ desc->type ];
            vDecl.m_attributes[ i ].m_size          = desc->size / toAttribSize[ desc->type ];
            vDecl.m_attributes[ i ].m_normalized    = toAttribNormalized[ desc->type ];
            vDecl.m_attributes[ i ].m_offset        = desc->offset;
            vDecl.m_size                            += desc->size;
        }

        m_vertexBuffer = RenderDevice::CreateVertexBuffer( header->vertexDataSize, ptr, BU_STATIC );
        if ( !m_vertexBuffer )
            return false;

        ptr += header->vertexDataSize;

        CARBON_ASSERT( header->subMeshCount < ms_maxSubMeshCount );
        for ( m_subMeshCount=0; m_subMeshCount<header->subMeshCount; ++m_subMeshCount )
        {
            SizeT count = *((U32*)ptr);
            ptr += sizeof(U32);

            SizeT size = count * DataTypeSize[ m_indexType ];

            SubMesh& sub_mesh       = m_subMeshes[ m_subMeshCount ];
            sub_mesh.m_indexCount   = count;

            sub_mesh.m_indexBuffer  = RenderDevice::CreateIndexBuffer( size, ptr, BU_STATIC );
            if ( !sub_mesh.m_indexBuffer )
                return false;

            sub_mesh.m_vertexArray  = RenderDevice::CreateVertexArray( m_vertexDecl, m_vertexBuffer, sub_mesh.m_indexBuffer );
            if ( !sub_mesh.m_indexBuffer )
                return false;

            ptr += size;

            U32 materialId = *((U32*)ptr);
            ptr += sizeof(U32);

            sub_mesh.m_material     = ProgramCache::CreateMaterial( materialId );
        }

        return true;
    }

    void MeshResource::Unload()
    {
        SubMesh * it = m_subMeshes;
        SubMesh * end = m_subMeshes + m_subMeshCount;
        for ( ; it != end; ++it )
        {
            RenderDevice::DestroyVertexArray( it->m_vertexArray );
            RenderDevice::DestroyBuffer( it->m_indexBuffer );
            it->m_material = 0;
        }

        RenderDevice::DestroyBuffer( m_vertexBuffer );
    }
}
