#pragma once
#ifndef _GRAPHIC_MESHRESOURCE_H
#define _GRAPHIC_MESHRESOURCE_H

#include "Graphic/DLL.h"

#include "Core/Resource.h"
#include "Graphic/RenderDevice.h"

#include "Core/FixedArray.h"

namespace Graphic
{
    class _GraphicExport MeshResource : public Core::Resource
    {
    public:
        static const SizeT  ms_maxSubMeshCount = 32;

        struct SubMesh
        {
            Handle  m_vertexArray;
            Handle  m_indexBuffer;
            SizeT   m_indexCount;
        };

        typedef Core::FixedArray< SubMesh, ms_maxSubMeshCount >   SubMeshArray;

    public:
        MeshResource();
        ~MeshResource();

        PrimitiveType               GetPrimitive() const;
        Handle                      GetVertexBuffer() const;
        const VertexDeclaration&    GetVertexDecl() const;
        SizeT                       GetVertexCount() const;
        DataType                    GetIndexType() const;
        const SubMeshArray&         GetSubMeshes() const;

    protected:
        void Load( const void * data );

    private:
        PrimitiveType       m_primitive;
        Handle              m_vertexBuffer;
        VertexDeclaration   m_vertexDecl;
        SizeT               m_vertexCount;
        DataType            m_indexType;
        SubMeshArray        m_subMeshes;
    };
}

#endif // _GRAPHIC_MESHRESOURCE_H