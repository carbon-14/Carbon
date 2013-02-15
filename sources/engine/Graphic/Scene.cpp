#include "Graphic/Scene.h"

#include "Graphic/RenderDevice.h"

#include "Core/Matrix.h"

namespace Graphic
{
    Scene::Scene()
    {
        m_uniformBuffer = RenderDevice::CreateUniformBuffer( sizeof(Parameters), &m_parameters, BU_STREAM );
    }

    Scene::~Scene()
    {
        RenderDevice::DestroyBuffer( m_uniformBuffer );
    }

    void Scene::Update()
    {
        void * data = RenderDevice::MapUniformBuffer( m_uniformBuffer, BA_WRITE_ONLY );
        MemoryUtils::MemCpy( data, &m_parameters, sizeof(Parameters) );
        RenderDevice::UnmapUniformBuffer();
    }

    void Scene::AddMesh( const Mesh * mesh )
    {
        m_meshes.PushBack( mesh );
    }

    const Scene::MeshArray& Scene::GetMeshes() const
    {
        return m_meshes;
    }

    void Scene::Clear()
    {
        m_meshes.Clear();
    }

    void Scene::SetParameters( const Parameters& parameters )
    {
        m_parameters = parameters;
    }

    const Scene::Parameters& Scene::GetParameters() const
    {
        return m_parameters;
    }

    Handle Scene::GetUniformBuffer() const
    {
        return m_uniformBuffer;
    }
}
