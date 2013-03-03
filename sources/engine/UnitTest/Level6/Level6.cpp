#include "UnitTest/Level6/Level6.h"

#include "Core/ResourceManager.h"

#include "Graphic/RenderList.h"
#include "Graphic/RenderGeometry.h"
#include "Graphic/TextureResource.h"
#include "Graphic/MeshResource.h"

#include "Core/Math.h"
#include "Core/Matrix.h"
#include "Core/Quaternion.h"

#include "Core/TimeUtils.h"

using namespace Graphic;

namespace Level6_NS
{
    enum CameraMode
    {
        CM_Demo = 0,
        CM_Free,
        CM_FPS
    };

    const F32 moveSpeed = 10.0f;
    const F32 turnSpeed = 0.2f * Pi();

    F32 moveX = 0.0f;
    F32 moveY = 0.0f;

    CameraMode cameraMode = CM_Demo;

    F32 mousedX = 0.0f;
    F32 mousedY = 0.0f;

    Vector cameraPosition       = Vector4( 0.0f, 0.0f, 0.0f );
    Vector cameraOrientation    = Quaternion( UnitY(), -HalfPi() );

    const Vector gravity = Vector3( 0.0f, -1.0f, 0.0f );
    const Vector arenaMax = Vector4( 3.0f, 100.0f, 7.0f );
    const Vector arenaMin = Vector4( -19.0f, -15.0f, -7.0f );
    const Vector bodyOffset = Vector3( 0.0f, 2.0f, 0.0f );
    const F32 groundHeight = -15.0f;
    const F32 bodyWalkSpeed = 7.5f;
    const F32 bodyRunSpeed = 15.0f;
    const F32 friction = 10.0f;

    Vector bodyPosition = Vector4( 0.0f, 0.0f, 0.0f );
    Vector bodySpeed    = Zero4();
    F32 bodyMass        = 50.0f;
    F32 bodyMaxSpeed    = bodyWalkSpeed;
    Bool bodyFall       = true;
    Bool bodyJump       = false;
    F32 bodyJumpForce   = 10.0f;

    Bool useFlashLight  = false;

    Handle cameraParameters;
    Handle ambientParameters;
    Handle lightParameters;
    Handle flashParameters;

    struct CameraData
    {
        Matrix  viewProjMatrix;
        Vector  position;
    };

    struct AmbientData
    {
        Vector  groundColor;
        Vector  skyColor;
    };

    struct LightData
    {
        Vector  position;
        Vector  direction;
        Vector  color;
        F32     radius;
    };

    class RenderMesh : public RenderGeometry
    {
    public:
        void Draw() const
        {
            RenderDevice::BeginGeometry( m_vertexArray );
            RenderDevice::DrawIndexed( m_primitive, m_indexCount, m_indexType );
            RenderDevice::EndGeometry();
        }

        PrimitiveType           m_primitive;
        DataType                m_indexType;

        Handle                  m_vertexArray;
        SizeT                   m_indexCount;
    };

    class MeshRenderer
    {
    public:
        MeshRenderer()
            : m_isFinalized( false )
        {
        }

        void Initialize()
        {
            m_mesh = ResourceManager::Create< MeshResource >( "level6/level6.bmh" );
        }

        void Finalize()
        {
            m_uniformBuffers[0].m_handle    = cameraParameters;
            m_uniformBuffers[0].m_index     = 0;
            m_uniformBuffers[1].m_handle    = ambientParameters;
            m_uniformBuffers[1].m_index     = 1;
            m_uniformBuffers[2].m_handle    = lightParameters;
            m_uniformBuffers[2].m_index     = 2;
            m_uniformBuffers[3].m_handle    = flashParameters;
            m_uniformBuffers[3].m_index     = 3;

            const MeshResource::SubMesh * sub_mesh = m_mesh->GetSubMeshes();
            const SizeT count = m_mesh->GetSubMeshCount();
            for ( m_subMeshCount=0; m_subMeshCount<count; ++m_subMeshCount, ++sub_mesh )
            {
                RenderMesh& geom    = m_geometry[ m_subMeshCount ];
                geom.m_primitive    = m_mesh->GetPrimitive();
                geom.m_indexType    = m_mesh->GetIndexType();
                geom.m_vertexArray  = sub_mesh->m_vertexArray;
                geom.m_indexCount   = sub_mesh->m_indexCount;

                const MaterialResource * material = sub_mesh->m_material.ConstPtr();

                m_program[ m_subMeshCount ] = material->GetProgram();

                SizeT& texCount = m_textureCount[ m_subMeshCount ];
                for ( texCount=0; texCount<material->GetTextureCount(); ++texCount )
                {
                    const MaterialResource::Texture& texture = material->GetTexture( texCount );

                    LayoutHandle& texUnit   = m_textureUnits[ m_subMeshCount ][ texCount ];
                    texUnit.m_handle        = texture.m_resource->GetTexture();
                    texUnit.m_index         = texture.m_index;
                }
            }

            m_isFinalized = true;
        }

        void Render( RenderList& renderList )
        {
            if ( ! m_isFinalized )
            {
                if ( m_mesh->IsReady() )
                {
                    Finalize();
                }
            }

            if ( m_isFinalized )
            {
                RenderElement element;

                element.m_textureCount  = 0;

                for ( element.m_uniformBufferCount = 0; element.m_uniformBufferCount<4; ++element.m_uniformBufferCount )
                {
                    element.m_uniformBuffers[ element.m_uniformBufferCount ] = m_uniformBuffers[ element.m_uniformBufferCount ];
                }

                for ( SizeT i=0; i<m_subMeshCount; ++i )
                {
                    element.m_program = m_program[ i ];
                    for ( element.m_textureCount = 0; element.m_textureCount<m_textureCount[ i ]; ++element.m_textureCount )
                    {
                        element.m_textures[ element.m_textureCount ] = m_textureUnits[i][element.m_textureCount];
                    }
                    element.m_geometry = &m_geometry[i];

                    renderList.Push( element );
                }
            }
        }

        void Destroy()
        {
            m_mesh = 0;
        }

    private:
        SharedPtr< MeshResource >   m_mesh;
        Bool                        m_isFinalized;

        SizeT                       m_subMeshCount;
        ProgramHandle               m_program[ MeshResource::ms_maxSubMeshCount ];
        RenderMesh                  m_geometry[ MeshResource::ms_maxSubMeshCount ];
        LayoutHandle                m_textureUnits[ MeshResource::ms_maxSubMeshCount ][ s_maxTextureUnitCount ];
        SizeT                       m_textureCount[ MeshResource::ms_maxSubMeshCount ];

        LayoutHandle                m_uniformBuffers[4];
    };

    struct SphereParameters
    {
        Matrix  m_world;
        Vector  m_emissiveColor;
    };

    class SphereRenderer
    {
    public:
        SphereRenderer()
            : m_isFinalized(false)
        {
        }

        void Initialize()
        {
            m_mesh          = ResourceManager::Create< MeshResource >( "level6/sphere.bmh" );

            m_textures[0]   = ResourceManager::Create< TextureResource >( "level6/crack_c.btx" );
            m_textures[1]   = ResourceManager::Create< TextureResource >( "level6/crack_n.btx" );

            m_position      = Vector4( 5.0f, -10.0f, 0.0f );
            m_scale         = One4();
            m_orientation   = Quaternion( Normalize( Vector3( 1.0f, 1.0f, -1.0f ) ), 0.0f );
        }

        void Finalize()
        {
            m_uniformBuffers[0].m_handle    = cameraParameters;
            m_uniformBuffers[0].m_index     = 0;
            m_uniformBuffers[1].m_handle    = ambientParameters;
            m_uniformBuffers[1].m_index     = 1;
            m_uniformBuffers[2].m_handle    = lightParameters;
            m_uniformBuffers[2].m_index     = 2;
            m_uniformBuffers[3].m_handle    = flashParameters;
            m_uniformBuffers[3].m_index     = 3;
            m_uniformBuffers[4].m_handle    = RenderDevice::CreateUniformBuffer( sizeof(SphereParameters), NULL, BU_STREAM );
            m_uniformBuffers[4].m_index     = 4;

            const MeshResource::SubMesh * sub_mesh = m_mesh->GetSubMeshes();
            RenderMesh& geom    = m_geometry;
            geom.m_primitive    = m_mesh->GetPrimitive();
            geom.m_indexType    = m_mesh->GetIndexType();
            geom.m_vertexArray  = sub_mesh->m_vertexArray;
            geom.m_indexCount   = sub_mesh->m_indexCount;

            m_program = sub_mesh->m_material.ConstPtr()->GetProgram();

            for ( SizeT texCount=0; texCount<2; ++texCount )
            {
                LayoutHandle& texUnit   = m_textureUnits[ texCount ];
                texUnit.m_handle        = m_textures[ texCount ]->GetTexture();
                texUnit.m_index         = texCount;
            }

            m_isFinalized = true;
        }

        void Render( RenderList& renderList, F32 time )
        {
            m_orientation = Quaternion( Normalize( Vector3( 1.0f, 1.0f, -1.0f ) ), 0.1f * time );

            if ( ! m_isFinalized )
            {
                if ( m_textures[0]->IsReady() && m_textures[1]->IsReady() && m_mesh->IsReady() )
                {
                    Finalize();
                }
            }

            if ( m_isFinalized )
            {
                Vector emissive = Vector4( 10.0f, 5.0f, 2.5f, 1.0f ) / SquareLength( m_position - cameraPosition );

                SphereParameters * params = static_cast< SphereParameters * >( RenderDevice::MapUniformBuffer( m_uniformBuffers[4].m_handle, BA_WRITE_ONLY ) );

                params->m_world = RMatrix( m_orientation );
                Scale( params->m_world, m_scale );
                params->m_world.m_column[3] = m_position;

                params->m_emissiveColor = emissive;

                RenderDevice::UnmapUniformBuffer( );

                RenderElement element;

                for ( element.m_uniformBufferCount = 0; element.m_uniformBufferCount<5; ++element.m_uniformBufferCount )
                {
                    element.m_uniformBuffers[ element.m_uniformBufferCount ] = m_uniformBuffers[ element.m_uniformBufferCount ];
                }

                for ( element.m_textureCount = 0; element.m_textureCount<2; ++element.m_textureCount )
                {
                    element.m_textures[ element.m_textureCount ] = m_textureUnits[ element.m_textureCount ];
                }

                for ( SizeT i=0; i<m_mesh->GetSubMeshCount(); ++i )
                {
                    element.m_program = m_program;
                    element.m_geometry = &m_geometry;

                    renderList.Push( element );
                }
            }
        }

        void Destroy()
        {
            m_mesh = 0;

            m_textures[0] = 0;
            m_textures[1] = 0;

            RenderDevice::DestroyBuffer( m_uniformBuffers[4].m_handle );
        }

    private:
        SharedPtr< MeshResource >       m_mesh;
        SharedPtr< TextureResource >    m_textures[2];
        Bool                            m_isFinalized;

        SizeT                           m_subMeshCount;
        ProgramHandle                   m_program;
        RenderMesh                      m_geometry;
        LayoutHandle                    m_textureUnits[2];
        LayoutHandle                    m_uniformBuffers[5];

        Vector                          m_position;
        Vector                          m_scale;
        Vector                          m_orientation;
    };

    MeshRenderer    meshRenderer;
    SphereRenderer  sphereRenderer;

    void UpdateGlobalUniformData( F32 time, F32 elapsedTime, F32 frameRatio )
    {        
        F32 lightRatio = 0.5f + 0.5f * Sin( time );
        const Vector color = Vector4( 1.0f, 0.6881f, 0.5317f, 1.0f );

        Vector lightColor = Lerp( color, Swizzle< 2, 1, 0, 3 >( color ), Splat(lightRatio) );

        LightData  * light = static_cast< LightData * >( RenderDevice::MapUniformBuffer( lightParameters, BA_WRITE_ONLY ) );

        light->color = lightColor;

        RenderDevice::UnmapUniformBuffer( );

        switch ( cameraMode )
        {
        case CM_Demo:
            {
                cameraPosition  = Vector4( -15.0f * Cos( time * 0.1f ), -11.5f, 1.0f );
                Vector cam_at   = Vector4( 5.0f, -10.0f, 0.0f );

                Vector cam_dir      = Normalize( cam_at - cameraPosition );
                F128 dir;
                Store( dir, cam_dir );

                F32 angleX = ASin( dir[1] );

                Vector cam_proj     = Normalize( Select( cam_dir, Zero4(), Mask< 0, 1, 0, 1 >() ) );
                F128 proj;
                Store( proj, cam_proj );

                F32 angleY = ACos( proj[0] );
                if ( proj[2] > 0.0f )
                    angleY = -angleY;

                cameraOrientation = MulQuat( Quaternion( UnitY(), angleY - HalfPi() ), Quaternion( UnitX(), angleX ) );

                break;
            }
        case CM_Free:
            {
                Vector rotateY = Quaternion( UnitY(), -mousedX * elapsedTime * turnSpeed );
                mousedX = 0.0f;

                Vector rotateX = Quaternion( UnitX(), -mousedY * elapsedTime * turnSpeed );
                mousedY = 0.0f;

                cameraOrientation = MulQuat( cameraOrientation, rotateX );
                cameraOrientation = MulQuat( rotateY, cameraOrientation );

                Matrix ori = RMatrix( cameraOrientation );

                cameraPosition = cameraPosition - Splat(moveY * elapsedTime * moveSpeed) * ori.m_column[2];
                cameraPosition = cameraPosition + Splat(moveX * elapsedTime * moveSpeed) * ori.m_column[0];

                break;
            }
        case CM_FPS:
            {
                Vector rotateY = Quaternion( UnitY(), -mousedX * elapsedTime * turnSpeed );
                mousedX = 0.0f;

                Vector rotateX = Quaternion( UnitX(), -mousedY * elapsedTime * turnSpeed );
                mousedY = 0.0f;

                Vector rotate = MulQuat( rotateY, rotateX );
                cameraOrientation = MulQuat( cameraOrientation, rotateX );
                cameraOrientation = MulQuat( rotateY, cameraOrientation );

                if ( bodyFall )
                {
                    Vector force = gravity * Splat(bodyMass);
                    bodySpeed = bodySpeed + force * Splat(elapsedTime);
                    bodyPosition = bodyPosition + bodySpeed * Splat(elapsedTime);
                }
                else
                {
                    Vector move = Zero4();
                    if ( moveX != 0.0f || moveY != 0.0f )
                    {
                        Matrix ori = RMatrix( cameraOrientation );
                        Vector right = Splat( moveX ) * Select( ori.m_column[0], Zero4(), Mask< 0, 1, 0, 1 >() );
                        Vector front = Splat( moveY ) * Select( ori.m_column[2], Zero4(), Mask< 0, 1, 0, 1 >() );
                        move = right - front;
                    }

                    Vector force = Splat(bodyMaxSpeed) * move - bodySpeed;

                    bodySpeed = bodySpeed + force * Splat(friction*elapsedTime);

                    if ( bodyJump )
                    {
                        bodySpeed = bodySpeed + Normalize( UnitY() + Splat(0.1f) * move ) * Splat(bodyJumpForce);
                        bodyFall = true;
                        bodyJump = false;
                    }

                    bodyPosition = bodyPosition + bodySpeed * Splat(elapsedTime);
                }

                bodyPosition = Min( bodyPosition, arenaMax );
                bodyPosition = Max( bodyPosition, arenaMin );
                F128 pos;
                Store( pos, bodyPosition );
                if ( pos[1] <= groundHeight )
                {
                    bodyFall    = false;
                }

                cameraPosition = bodyPosition + Vector3( 0.0f, 2.0f, 0.0f );

                break;
            }
        }

        Matrix cam_base = RMatrix( cameraOrientation );
        cam_base.m_column[3] = cameraPosition;

        LightData  * flash = static_cast< LightData * >( RenderDevice::MapUniformBuffer( flashParameters, BA_WRITE_ONLY ) );

        flash->position     = cam_base.m_column[3];
        flash->direction    = -cam_base.m_column[2];
        flash->color        = useFlashLight ? One4() : Zero4();

        RenderDevice::UnmapUniformBuffer( );

        Matrix view = Inverse( cam_base );

        F32 n       = 0.25f;
        F32 f       = 50.0f;
        F32 fov     = HalfPi();                 // need more ??? :)

        F32 cotan   = 1.0f / Tan( 0.5f * fov ) ;

        Matrix proj;
        proj.m_column[0] = Vector4( cotan   , 0.0f                  , 0.0f                          , 0.0f  );
        proj.m_column[1] = Vector4( 0.0f    , cotan * frameRatio    , 0.0f                          , 0.0f  );
        proj.m_column[2] = Vector4( 0.0f    , 0.0f                  , ( f + n ) / ( n - f )         , -1.0f );
        proj.m_column[3] = Vector4( 0.0f    , 0.0f                  , ( 2.0f * n * f ) / ( n - f )  , 0.0f  );

        CameraData  * cam = static_cast< CameraData * >( RenderDevice::MapUniformBuffer( cameraParameters, BA_WRITE_ONLY ) );

        cam->viewProjMatrix = Mul( proj, view );
        cam->position       = cam_base.m_column[3];

        RenderDevice::UnmapUniformBuffer( );
    }
}

using namespace Level6_NS;

Level6::Level6( const RenderWindow& window )
    : Application( window )
{
}

void Level6::ProcessInputs( RAWINPUT * raw )
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
            case VK_F1:
                cameraMode = CM_Demo;
                break;
            case VK_F2:
                cameraMode = CM_Free;
                break;
            case VK_F3:
                cameraMode = CM_FPS;
                bodyPosition = cameraPosition - bodyOffset;
                bodySpeed = Zero4();
                bodyFall = true;
                break;
            case VK_SHIFT:
                bodyMaxSpeed = bodyRunSpeed;
                break;
            case VK_SPACE:
                if ( !bodyFall )
                    bodyJump = true;
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
            case 'F' :
                useFlashLight = ! useFlashLight;
                break;
            }
        }
        else if ( raw->data.keyboard.Message == WM_KEYUP )
        {
            switch ( raw->data.keyboard.VKey )
            {
                case VK_SHIFT:
                    bodyMaxSpeed = bodyWalkSpeed;
                    break;
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

void Level6::PreExecute()
{
    RenderState opaque;
    opaque.m_enableDepthTest = true;
    opaque.m_enableCullFace  = true;

    m_renderList.SetRenderState( opaque );
    m_renderList.SetClearMask( RM_ALL );
    m_renderList.SetSRGBWrite( true );

    cameraParameters = RenderDevice::CreateUniformBuffer( sizeof(CameraData),NULL, BU_STREAM );

    AmbientData ambient;
    ambient.groundColor = Vector4( 0.01f, 0.01f, 0.01f, 1.0f );
    ambient.skyColor    = Vector4( 0.0074f, 0.0133f, 0.025f, 1.0f );

    ambientParameters = RenderDevice::CreateUniformBuffer( sizeof(AmbientData),&ambient, BU_STATIC );

    LightData light;
    light.position = Vector4( 10.0f, 0.0f, 0.0f );
    light.radius = 20.0f;

    lightParameters = RenderDevice::CreateUniformBuffer( sizeof(LightData),&light, BU_STREAM );

    LightData flash;
    flash.color = Zero4();
    flash.radius = 15.0f;

    flashParameters = RenderDevice::CreateUniformBuffer( sizeof(LightData),&flash, BU_STREAM );

    meshRenderer.Initialize();

    sphereRenderer.Initialize();
}

void Level6::PostExecute()
{
    sphereRenderer.Destroy();
    meshRenderer.Destroy();

    m_renderCache.Clear();

    RenderDevice::DestroyBuffer( flashParameters );
    RenderDevice::DestroyBuffer( lightParameters );
    RenderDevice::DestroyBuffer( ambientParameters );
    RenderDevice::DestroyBuffer( cameraParameters );
}

void Level6::Execute()
{
    F32 time        = static_cast< F32 >( m_clockTicks * TimeUtils::ClockPeriod() );
    F32 elapsedTime = static_cast< F32 >( m_frameTicks * TimeUtils::ClockPeriod() );
    F32 frameRatio  = static_cast< F32 >( m_window.width ) / static_cast< F32 >( m_window.height );

    UpdateGlobalUniformData( time, elapsedTime, frameRatio );

    meshRenderer.Render( m_renderList );
    sphereRenderer.Render( m_renderList, time );

    m_renderList.Draw( m_renderCache );
}

