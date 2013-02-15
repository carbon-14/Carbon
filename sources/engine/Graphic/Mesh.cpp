#include "Graphic/Mesh.h"

#include "Graphic/RenderDevice.h"

namespace Graphic
{
    void Mesh::Geometry::Draw() const
    {
        RenderDevice::BeginGeometry( m_vertexDecl, m_vertexArray, m_indexBuffer );
        RenderDevice::DrawIndexed( m_primitive, m_indexCount, m_indexType );
        RenderDevice::EndGeometry( m_vertexDecl );
    }

    Mesh::Mesh()
        : m_geomCount( 0 )
        , m_isFinalized(false)
    {
        m_uniformBuffer = RenderDevice::CreateUniformBuffer( sizeof(Parameters), NULL, BU_DYNAMIC );
    }

    Mesh::~Mesh()
    {
        RenderDevice::DestroyBuffer( m_uniformBuffer );
    }

    void Mesh::Update()
    {
        void * data = RenderDevice::MapUniformBuffer( m_uniformBuffer, BA_WRITE_ONLY );
        MemoryUtils::MemCpy( data, &m_parameters, sizeof(Parameters) );
        RenderDevice::UnmapUniformBuffer();
    }

    void Mesh::SetParameters( const Parameters& parameters )
    {
        m_parameters = parameters;
    }

    const Mesh::Parameters& Mesh::GetParameters() const
    {
        return m_parameters;
    }

    void Mesh::SetResource( MeshResource * resource )
    {
        m_resource = resource;
    }

    const MeshResource * Mesh::GetResource() const
    {
        return m_resource.ConstPtr();
    }

    const RenderGeometry * Mesh::GetGeom( SizeT index ) const
    {
        CARBON_ASSERT( index < m_geomCount );
        return m_geoms + index;
    }

    SizeT Mesh::GetGeomCount() const
    {
        return m_geomCount;
    }

    Handle Mesh::GetUniformBuffer() const
    {
        return m_uniformBuffer;
    }

    void Mesh::Finalize()
    {
        for ( m_geomCount=0; m_geomCount<m_resource->GetSubMeshCount(); ++m_geomCount )
        {
            Geometry& geom      = m_geoms[ m_geomCount ];

            geom.m_primitive    = m_resource->GetPrimitive();
            geom.m_vertexDecl   = m_resource->GetVertexDecl();
            geom.m_indexType    = m_resource->GetIndexType();

            const MeshResource::SubMesh& sub_mesh = m_resource->GetSubMeshes()[ m_geomCount ];
            geom.m_vertexArray  = sub_mesh.m_vertexArray;
            geom.m_indexBuffer  = sub_mesh.m_indexBuffer;
            geom.m_indexCount   = sub_mesh.m_indexCount;
        }

        m_isFinalized = true;
    }

    Bool Mesh::IsFinalized() const
    {
        return m_isFinalized;
    }
}
