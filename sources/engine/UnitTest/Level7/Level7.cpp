#include "UnitTest/Level7/Level7.h"

#include "Core/ResourceManager.h"
#include "Core/Vector.h"
#include "Core/Matrix.h"
#include "Core/Quaternion.h"
#include "Core/TimeUtils.h"

#include "Graphic/Scene.h"
#include "Graphic/Mesh.h"
#include "Graphic/Light.h"
#include "Graphic/Camera.h"
#include "Graphic/QuadGeometry.h"
#include <random>

using namespace Graphic;

namespace Level7_NS
{
    Mesh *  m_mesh = 0;
    Mesh *  m_sphere = 0;
    Scene * m_scene = 0;

    Array< Light * > m_lights; 

    Bool    scene_ready;

    Camera * m_camera = 0;
    
    const F32 moveSpeed = 10.0f;
    const F32 turnSpeed = 0.2f * Pi();

    F32 moveX = 0.0f;
    F32 moveY = 0.0f;

    F32 mousedX = 0.0f;
    F32 mousedY = 0.0f;
}

using namespace Level7_NS;

Level7::Level7( const RenderWindow& window )
    : Application( window )
{
}

void Level7::ProcessInputs( RAWINPUT * raw )
{
    if ( raw->header.dwType == RIM_TYPEKEYBOARD ) 
    {
        if ( raw->data.keyboard.Message == WM_KEYDOWN )
        {
            switch ( raw->data.keyboard.VKey )
            {
            case VK_F4:
                ProgramCache::NotifySourceChange();
                break;
            case 'Z':
                moveY = Min( 1.0f, moveY + 1.0f );
                break;
            case 'Q':
                moveX = Max( -1.0f, moveX - 1.0f );
                break;
            case 'S':
                moveY = Max( -1.0f, moveY - 1.0f );
                break;
            case 'D':
                moveX = Min( 1.0f, moveX + 1.0f );
                break;
            }
        }
        else if ( raw->data.keyboard.Message == WM_KEYUP )
        {
            switch ( raw->data.keyboard.VKey )
            {
                case 'Z':
                    moveY = Max( -1.0f, moveY - 1.0f );
                    break;
                case 'Q':
                    moveX = Min( 1.0f, moveX + 1.0f );
                    break;
                case 'S':
                    moveY = Min( 1.0f, moveY + 1.0f );
                    break;
                case 'D':
                    moveX = Max( -1.0f, moveX - 1.0f );
                    break;
            }
        }
    }
    else if ( raw->header.dwType == RIM_TYPEMOUSE )
    {
        mousedX = (F32)raw->data.mouse.lLastX;
        mousedY = (F32)raw->data.mouse.lLastY;
    }
}

void Level7::PreExecute()
{
    QuadGeometry::GetInstance().Initialize();

    m_frameRenderer.Initialize();

    m_scene = MemoryManager::New< Scene >();

    {
        m_mesh = MemoryManager::New< Mesh >();
        m_mesh->SetResource( ResourceManager::Create< MeshResource >( "sibenik/sibenik.bmh" ) );
        m_mesh->m_position = Vector4( 0.0f, 0.0f, 0.0f );
        m_mesh->m_scale = One4();
        m_mesh->m_orientation = IdentityQuat();
        m_mesh->Update();

        m_scene->AddMesh( m_mesh );
    }

    {
        m_sphere = MemoryManager::New< Mesh >();
        m_sphere->SetResource( ResourceManager::Create< MeshResource >( "sphere/sphere.bmh" ) );
        m_sphere->m_position = Vector4( 5.0f, -10.0f, 0.0f );
        m_sphere->m_scale = One4();
        m_sphere->m_orientation = Quaternion( Normalize( Vector3( 1.0f, 1.0f, -1.0f ) ), 0.0f );
        m_sphere->Update();

        m_scene->AddMesh( m_sphere );
    }

    {
        SizeT X = 9;
        SizeT Y = 5;
        SizeT Z = 3;

        const Vector light_spacing = Splat( 6.0f );
        const Vector light_offset  = Vector4( 0.0f, 0.0f, 0.0f ) - light_spacing * Vector4( 0.5f * (X-1), 0.5f * (Y-1), 0.5f * (Z-1) );

        m_lights.Reserve( X * Y * Z );

        for ( SizeT i=0; i<X; ++i )
        {
            for ( SizeT j=0; j<Y; ++j )
            {
                for ( SizeT k=0; k<Z; ++k )
                {
                    F32 x = (F32)i;
                    F32 y = (F32)j;
                    F32 z = (F32)k;

                    F32 lightRatio = ((F32)rand())/RAND_MAX;
                    const Vector color = Vector4( 1.0f, 0.6881f, 0.5317f, 1.0f );

                    Vector lightColor = Lerp( color, Swizzle< 2, 1, 0, 3 >( color ), Splat(lightRatio) );

                    F32 angleX = TwoPi()*((F32)rand())/RAND_MAX;
                    F32 angleY = TwoPi()*((F32)rand())/RAND_MAX;

                    Light * light               = MemoryManager::New< Light >();
                    light->m_value              = lightColor;
                    light->m_orientation        = MulQuat( Quaternion( UnitY(), angleY ), Quaternion( UnitX(), angleX ) );
                    light->m_position           = Vector4( x, y, z ) * light_spacing + light_offset;
                    light->m_radius             = 6.0f;
                    light->m_spotInAngle        = 0.0f;
                    light->m_spotOutAngle       = HalfPi();
                    light->m_directionalWidth   = 10.0f;
                    light->m_directionalHeight  = 5.0f;
                    light->m_type               = LT_OMNI;

                    m_lights.PushBack( light );
                    m_scene->AddLight( light );
                }
            }
        }
    }

    {
        m_camera = MemoryManager::New< Camera >();

        m_camera->m_position        = Vector4( -15.0f, -11.5f, 1.0f );
        m_camera->m_orientation     = Quaternion( UnitY(), -HalfPi() );

        m_camera->m_near            = 0.25f;
        m_camera->m_far             = 100.0f;
        m_camera->m_fov             = HalfPi();
        m_camera->m_aspectRatio     = static_cast< F32 >( m_window.width ) / static_cast< F32 >( m_window.height );

        m_camera->m_projectionType  = PT_PERSPECTIVE;

        m_camera->Update();
    }

    FrameRenderer::InitializeFrameContext( m_frameContext, m_window.width, m_window.height, m_camera );

    scene_ready = false;
}

void Level7::PostExecute()
{
    FrameRenderer::DestroyFrameContext( m_frameContext );

    m_scene->Clear();

    MemoryManager::Delete( m_camera );
    
    Array< Light * >::Iterator light_it = m_lights.Begin();
    Array< Light * >::Iterator light_end = m_lights.End();
    for ( ; light_it != light_end; ++light_it )
    {
        MemoryManager::Delete( *light_it );
    }
    m_lights.Clear();

    MemoryManager::Delete( m_sphere );
    MemoryManager::Delete( m_mesh );
    MemoryManager::Delete( m_scene );

    m_frameRenderer.Destroy();

    QuadGeometry::GetInstance().Destroy();
}

void Level7::Execute()
{
    if ( scene_ready )
    {
        F32 time = static_cast< F32 >( m_clockTicks * TimeUtils::ClockPeriod() );
        F32 elapsedTime = static_cast< F32 >( m_frameTicks * TimeUtils::ClockPeriod() );

        Vector rotateY = Quaternion( UnitY(), -mousedX * elapsedTime * turnSpeed );
        mousedX = 0.0f;

        Vector rotateX = Quaternion( UnitX(), -mousedY * elapsedTime * turnSpeed );
        mousedY = 0.0f;

        m_camera->m_orientation = MulQuat( m_camera->m_orientation, rotateX );
        m_camera->m_orientation = MulQuat( rotateY, m_camera->m_orientation );

        Matrix ori = RMatrix( m_camera->m_orientation );

        m_camera->m_position = m_camera->m_position - Splat(moveY * elapsedTime * moveSpeed) * ori.m_column[2];
        m_camera->m_position = m_camera->m_position + Splat(moveX * elapsedTime * moveSpeed) * ori.m_column[0];

        m_camera->Update();

        m_sphere->m_orientation = Quaternion( Normalize( Vector3( 1.0f, 1.0f, -1.0f ) ), 0.1f * time );
        m_sphere->Update();

        m_frameRenderer.Render( m_scene, m_frameContext );
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

        if ( !m_sphere->IsFinalized() )
        {
            if ( m_sphere->GetResource()->IsReady() )
            {
                m_sphere->Finalize();
            }
            else
            {
                scene_ready = false;
            }
        }
    }
}

