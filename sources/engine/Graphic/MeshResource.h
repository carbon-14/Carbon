#pragma once
#ifndef _GRAPHIC_MESHRESOURCE_H
#define _GRAPHIC_MESHRESOURCE_H

#include "Graphic/DLL.h"

#include "Core/Resource.h"
#include "Graphic/MaterialResource.h"

namespace Graphic
{
    class MaterialResource;

    class _GraphicExport MeshResource : public Resource
    {
    public:
        static const SizeT  ms_maxSubMeshCount = 32;

        struct SubMesh
        {
            Handle                          m_vertexArray;
            Handle                          m_indexBuffer;
            SizeT                           m_indexCount;
            SharedPtr< MaterialResource >   m_material;
        };

    public:
        MeshResource();
        ~MeshResource();

        PrimitiveType               GetPrimitive() const;
        Handle                      GetVertexBuffer() const;
        const VertexDeclaration&    GetVertexDecl() const;
        SizeT                       GetVertexCount() const;
        DataType                    GetIndexType() const;
        const SubMesh *             GetSubMeshes() const;
        SizeT                       GetSubMeshCount() const;

    protected:
        Bool Load( const void * data );
        void Unload();

    private:
        PrimitiveType       m_primitive;
        Handle              m_vertexBuffer;
        VertexDeclaration   m_vertexDecl;
        SizeT               m_vertexCount;
        DataType            m_indexType;
        SubMesh             m_subMeshes[ ms_maxSubMeshCount ];
        SizeT               m_subMeshCount;
    };
}

#endif // _GRAPHIC_MESHRESOURCE_H