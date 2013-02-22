#pragma once
#ifndef _GRAPHIC_SCENE_H
#define _GRAPHIC_SCENE_H

#include "Graphic/DLL.h"

#include "Graphic/Types.h"

#include "Core/Array.h"
#include "Core/Vector.h"

namespace Graphic
{
    class Mesh;

    class _GraphicExport Scene
    {
    public:
        typedef Array< const Mesh * > MeshArray;

        struct Parameters
        {
            F128    m_viewMatrix[4];
            F128    m_projMatrix[4];
            F128    m_position;
        };

    public:
        Scene();
        ~Scene();

        void Update();
        
        void AddMesh( const Mesh * mesh );
        const MeshArray& GetMeshes() const;
        void Clear();

        void SetParameters( const Parameters& parameters );
        const Parameters& GetParameters() const;

        Handle GetUniformBuffer() const;

    private:
        MeshArray   m_meshes;
        Parameters  m_parameters;

        Handle      m_uniformBuffer;
    };
}

#endif // _GRAPHIC_SCENE_H


