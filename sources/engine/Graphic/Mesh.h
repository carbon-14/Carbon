#pragma once
#ifndef _GRAPHIC_MESH_H
#define _GRAPHIC_MESH_H

#include "Graphic/RenderGeometry.h"
#include "Graphic/MeshResource.h"

#include "Core/Matrix.h"

namespace Graphic
{
    class _GraphicExport Mesh
    {
    public:
        Mesh();
        ~Mesh();

        void Update();

        Vector m_position;
        Vector m_scale;
        Vector m_orientation;

        void SetResource( MeshResource * resource );
        const MeshResource * GetResource() const;

        const RenderGeometry * GetGeom( SizeT index ) const;
        SizeT GetGeomCount() const;

        Handle GetUniformBuffer() const;

        void Finalize();
        Bool IsFinalized() const;
        
    private:
        class Geometry : public RenderGeometry
        {
        public:
            void Draw() const;

            PrimitiveType       m_primitive;
            DataType            m_indexType;
            Handle              m_vertexArray;
            SizeT               m_indexCount;
        };

        SharedPtr< MeshResource >   m_resource;

        Geometry                    m_geoms[ MeshResource::ms_maxSubMeshCount ];
        SizeT                       m_geomCount;

        Handle                      m_uniformBuffer;

        Bool                        m_isFinalized;
    };
}

#endif // _GRAPHIC_MESH_H
