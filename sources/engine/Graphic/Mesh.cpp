#include "Graphic/Mesh.h"

#include "Graphic/RenderDevice.h"

namespace Graphic
{
    struct MeshParameters
    {
        Matrix  m_transform;
    };

    void Mesh::Geometry::Draw() const
    {
        RenderDevice::BeginGeometry( m_vertexArray );
        RenderDevice::DrawIndexed( m_primitive, m_indexCount, m_indexType );
        RenderDevice::EndGeometry();
    }

    Mesh::Mesh()
        : m_geomCount( 0 )
        , m_isFinalized(false)
    {
        m_uniformBuffer = RenderDevice::CreateUniformBuffer( sizeof(MeshParameters), 0, BU_DYNAMIC );
    }

    Mesh::~Mesh()
    {
        RenderDevice::DestroyBuffer( m_uniformBuffer );
    }

    void Mesh::Update()
    {
		MeshParameters * params	= reinterpret_cast< MeshParameters * >( RenderDevice::MapUniformBuffer( m_uniformBuffer, BA_WRITE_ONLY ) );

        params->m_transform = RMatrix( m_orientation );
        Scale( params->m_transform, m_scale );
        params->m_transform.m_column[3] = m_position;

        RenderDevice::UnmapUniformBuffer();
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
            geom.m_indexType    = m_resource->GetIndexType();

            const MeshResource::SubMesh& sub_mesh = m_resource->GetSubMeshes()[ m_geomCount ];
            geom.m_vertexArray  = sub_mesh.m_vertexArray;
            geom.m_indexCount   = sub_mesh.m_indexCount;
        }

        m_isFinalized = true;
    }

    Bool Mesh::IsFinalized() const
    {
        return m_isFinalized;
    }
}
