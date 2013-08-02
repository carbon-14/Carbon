#pragma once
#ifndef _GRAPHIC_SCENE_H
#define _GRAPHIC_SCENE_H

#include "Graphic/DLL.h"

#include "Core/Array.h"
#include "Core/Vector.h"
#include "Core/Matrix.h"

namespace Graphic
{
    class Mesh;
    class Light;
    class Camera;

    class _GraphicExport Scene
    {
    public:
        typedef Array< const Mesh * > MeshArray;
        typedef Array< const Light * > LightArray;

    public:
        Scene();
        ~Scene();
        
        void AddMesh( const Mesh * mesh );
        const MeshArray& GetMeshes() const;
        void AddLight( const Light * light );
        const LightArray& GetLights() const;
        void SetAmbientSkyLight( const Vector& light );
        const Vector& GetAmbientSkyLight() const;
        void SetAmbientGroundLight( const Vector& light );
        const Vector& GetAmbientGroundLight() const;
        void Clear();

    private:
        MeshArray   m_meshes;
        LightArray  m_lights;
        Vector      m_ambientSkyLight;
        Vector      m_ambientGroundLight;
    };
}

#endif // _GRAPHIC_SCENE_H


