#include "Graphic/Scene.h"

#include "Graphic/RenderDevice.h"

#include "Core/Matrix.h"

namespace Graphic
{
    Scene::Scene()
        : m_ambientSkyLight( Vector4( 0.0f, 0.0f, 0.0f ) )
        , m_ambientGroundLight( Vector4( 0.0f, 0.0f, 0.0f ) )
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

    void Scene::SetAmbientSkyLight( const Vector& light )
    {
        m_ambientSkyLight = light;
    }

    const Vector& Scene::GetAmbientSkyLight() const
    {
        return m_ambientSkyLight;
    }

    void Scene::SetAmbientGroundLight( const Vector& light )
    {
        m_ambientGroundLight = light;
    }

    const Vector& Scene::GetAmbientGroundLight() const
    {
        return m_ambientGroundLight;
    }

    void Scene::Clear()
    {
        m_meshes.Clear();
        m_lights.Clear();
    }
}
