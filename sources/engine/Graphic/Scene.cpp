#include "Graphic/Scene.h"

#include "Graphic/RenderDevice.h"

#include "Core/Matrix.h"

namespace Graphic
{
    Scene::Scene()
    {
    }

    Scene::~Scene()
    {
    }

    void Scene::AddMesh( const Mesh * mesh )
    {
        m_meshes.PushBack( mesh );
    }

    const Scene::MeshArray& Scene::GetMeshes() const
    {
        return m_meshes;
    }

    void Scene::AddLight( const Light * light )
    {
        m_lights.PushBack( light );
    }

    const Scene::LightArray& Scene::GetLights() const
    {
        return m_lights;
    }

    void Scene::Clear()
    {
        m_meshes.Clear();
        m_lights.Clear();
    }
}
