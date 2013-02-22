#include "UnitTest/Level7/Level7.h"

#include "Core/ResourceManager.h"
#include "Core/Vector.h"
#include "Core/Matrix.h"
#include "Core/Quaternion.h"

#include "Graphic/Scene.h"

using namespace Graphic;

namespace Level7_NS
{
    Mesh *  m_mesh = 0;
    Mesh *  m_sphere = 0;
    Scene * m_scene = 0;

    Bool    scene_ready;

    Vector cameraPosition       = Vector4( 0.0f, 0.0f, 0.0f );
    Vector cameraOrientation    = Quaternion( UnitY(), -HalfPi() );
}

using namespace Level7_NS;

Level7::Level7( const RenderWindow& window )
    : Application( window )
{
}

void Level7::ProcessInputs( RAWINPUT * raw )
{
}

void Level7::PreExecute()
{
    m_frameRenderer.Initialize();    

    {
        Mesh::Parameters mesh_params;
        Store( mesh_params.m_transform, Identity() );

        m_mesh = MemoryManager::New< Mesh >();
        m_mesh->SetResource( ResourceManager::Create< MeshResource >( "sibenik/sibenik.bmh" ) );
        m_mesh->SetParameters( mesh_params );
        m_mesh->Update();
    }

    /*{
        Mesh::Parameters sphere_params;
        Store( sphere_params.m_transform, Identity() );

        m_sphere = MemoryManager::New< Mesh >();
        m_sphere->SetResource( ResourceManager::Create< MeshResource >( "level7/sphere.bmh" ) );
        m_sphere->SetParameters( sphere_params );
        m_sphere->Update();
    }*/

    {
        Matrix cam_base = RMatrix( cameraOrientation );
        cam_base.m_column[3] = cameraPosition;

        Matrix view = Inverse( cam_base );

        F32 n       = 0.25f;
        F32 f       = 50.0f;
        F32 fov     = HalfPi();

        F32 cotan   = 1.0f / Tan( 0.5f * fov ) ;

        F32 frameRatio  = static_cast< F32 >( m_window.width ) / static_cast< F32 >( m_window.height );

        Matrix proj;
        proj.m_column[0] = Vector4( cotan   , 0.0f                  , 0.0f                          , 0.0f  );
        proj.m_column[1] = Vector4( 0.0f    , cotan * frameRatio    , 0.0f                          , 0.0f  );
        proj.m_column[2] = Vector4( 0.0f    , 0.0f                  , ( f + n ) / ( n - f )         , -1.0f );
        proj.m_column[3] = Vector4( 0.0f    , 0.0f                  , ( 2.0f * n * f ) / ( n - f )  , 0.0f  );

        Scene::Parameters scene_params;
        Store( scene_params.m_viewMatrix, view );
        Store( scene_params.m_projMatrix, proj );
        Store( scene_params.m_position, cameraPosition );

        m_scene = MemoryManager::New< Scene >();
        m_scene->SetParameters( scene_params );
        m_scene->Update();
        m_scene->AddMesh( m_mesh );
        //m_scene->AddMesh( m_sphere );
    }

    scene_ready = false;
}

void Level7::PostExecute()
{
    m_scene->Clear();

    //MemoryManager::Delete( m_sphere );
    MemoryManager::Delete( m_mesh );
    MemoryManager::Delete( m_scene );

    m_frameRenderer.Destroy();
}

void Level7::Execute()
{
    if ( scene_ready )
    {
        m_frameRenderer.Render( m_scene, m_window.width, m_window.height );
    }
    else
    {
        scene_ready = true;

        if ( !m_mesh->IsFinalized() )
        {
            if ( m_mesh->GetResource()->IsReady() )
            {
                m_mesh->Finalize();
            }
            else
            {
                scene_ready = false;
            }
        }

        /*if ( !m_sphere->IsFinalized() )
        {
            if ( m_sphere->GetResource()->IsReady() )
            {
                m_sphere->Finalize();
            }
            else
            {
                scene_ready = false;
            }
        }*/
    }
}

