#pragma once
#ifndef _GRAPHIC_MESH_H
#define _GRAPHIC_MESH_H

#include "Core/Matrix.h"

#include "Graphic/RenderGeometry.h"
#include "Graphic/MeshResource.h"

namespace Graphic
{
    class _GraphicExport Mesh
    {
    public:
        Mesh();

        void SetTransform( const Matrix& transform );
        const Matrix& GetTransform() const;

        void SetResource( MeshResource * resource );
        const MeshResource * GetResource() const;

        const RenderGeometry * GetGeoms() const;
        SizeT GetGeomCount() const;

        void Finalize();
        
    private:
        class Geometry : public RenderGeometry
        {
        public:
            void Draw() const;

            PrimitiveType       m_primitive;
            VertexDeclaration   m_vertexDecl;
            DataType            m_indexType;
            Handle              m_vertexArray;
            Handle              m_indexBuffer;
            SizeT               m_indexCount;
        };

        Matrix                      m_transform;
        SharedPtr< MeshResource >   m_resource;

        Geometry                    m_geoms[ MeshResource::ms_maxSubMeshCount ];
        SizeT                       m_geomCount;
    };
}

#endif // _GRAPHIC_MESH_H
