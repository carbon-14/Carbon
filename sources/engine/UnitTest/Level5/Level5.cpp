#include "UnitTest/Level5/Level5.h"
#include "UnitTest/Utils.h"

#include "Core/FileSystem.h"
#include "Core/ResourceManager.h"
#include "Core/Resource.h"

#include "Graphic/RenderDevice.h"
#include "Graphic/ProgramCache.h"
#include "Graphic/RenderList.h"
#include "Graphic/RenderGeometry.h"

#include "Core/Math.h"
#include "Core/Matrix.h"
#include "Core/Quaternion.h"

#include "Core/TimeUtils.h"

using namespace Core;
using namespace Graphic;

#ifndef HID_USAGE_PAGE_GENERIC
#define HID_USAGE_PAGE_GENERIC         ((USHORT) 0x01)
#endif
#ifndef HID_USAGE_GENERIC_MOUSE
#define HID_USAGE_GENERIC_MOUSE        ((USHORT) 0x02)
#endif

namespace Level5_NS
{
    const U32 FRAME_MAX_COUNT = 60;
    U64 clockTicks = 0;
    U32 frameCount = 0;
    U64 frameTime = 0;
    F32 time = 0.0f;

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

    const SizeT frameAllocatorSize = 10 * 1024;

    const U32 frameWidth    = 1280;
    const U32 frameHeight   = 720;
    const F32 frameRatio    = (F32)frameWidth/(F32)frameHeight;

    RenderDevice device3d;
    ProgramCache programCache;
    RenderList renderList;

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

    class FakeResource : public Resource
    {
    public:
        FakeResource( const Char * name ) : Resource(name) { m_state = CREATED; }
        void Load( void * ) { m_state = LOADED; }
    };

    struct TextureHeader
    {
        U32     internalFormat;
        U32     externalFormat;
        U32     mipMapCount;
        Bool    compressed;
    };

    struct LevelDesc
    {
        U32 size;
        U32 width;
        U32 height;
    };

    Handle LoadTexture( const Char * filename )
    {
        PathString path;
        FileSystem::BuildPathName( filename, path, FileSystem::PT_CACHE );

        void * data;
        SizeT size;
        FileSystem::Load( path, data, size );

        U8 * ptr = (U8*)data;

        TextureHeader * header = (TextureHeader*)ptr;

        ptr += sizeof(TextureHeader);

        const SizeT maxMipMapCount = 16;
        SizeT sizes[ maxMipMapCount ];
        SizeT widths[ maxMipMapCount ];
        SizeT heights[ maxMipMapCount ];
        void * datas[ maxMipMapCount ];

        SizeT count = ( header->mipMapCount < maxMipMapCount ) ? header->mipMapCount : maxMipMapCount;
        for ( SizeT i=0; i<count; ++i )
        {
            LevelDesc * desc = (LevelDesc*)ptr;
            ptr += sizeof(LevelDesc);

            sizes[ i ]      = desc->size;
            widths[ i ]     = desc->width;
            heights[ i ]    = desc->height;
            datas[ i ]      = ptr;
            ptr += desc->size;
        }

        Handle texture = RenderDevice::CreateTexture( header->internalFormat, header->externalFormat, count, header->compressed, sizes, widths, heights, datas );

        UnknownAllocator::Deallocate( data );

        return texture;
    }

    enum VType
    {
        VT_UBYTE    = 0,
        VT_SHORT,
        VT_HALF,
        VT_FLOAT,
        VT_NMVECT      // 2_10_10_10
    };

    const DataType toAttribType[] =
    {
        DT_U8,
        DT_S16,
        DT_F16,
        DT_F32,
        DT_S2_10_10_10
    };

    const SizeT toAttribSize[] =
    {
        1, 
        2, 
        2,
        4,
        1   // used to keep a size of 4 in vertex declaration
    };

    const Bool toAttribNormalized[] =
    {
        true,   // color
        false,
        false,
        false,
        true    // normal
    };

    enum IType
    {
        IT_UBYTE   = 0,
        IT_USHORT,
        IT_ULONG
    };

    const DataType toIndexType[] =
    {
        DT_U8,
        DT_U16,
        DT_U32
    };

    const SizeT toIndexSize[] =
    {
        1, 
        2, 
        4
    };

    struct MeshHeader
    {
        IType   indexType;
        U32     subMeshCount;
        U32     inputCount;
        U32     vertexDataSize;
    };

    struct MeshInput
    {
        VertexSemantic  semantic;
        VType           type;
        U32             size;
        U32             offset;
    };

    struct SubMesh
    {
        Handle  m_vertexArray;
        Handle  m_indexBuffer;
        SizeT   m_indexCount;
    };

    struct Mesh
    {
        PrimitiveType       m_primitive;
        Handle              m_vertexBuffer;
        VertexDeclaration   m_vertexDecl;
        SizeT               m_vertexCount;
        DataType            m_indexType;
        SizeT               m_subMeshCount;
        SubMesh             m_subMeshes[32];
    };

    Mesh LoadMesh( const Char * filename )
    {
        Mesh mesh;

        mesh.m_primitive = PT_TRIANGLES;

        PathString path;
        FileSystem::BuildPathName( filename, path, FileSystem::PT_CACHE );

        void * data;
        SizeT size;
        FileSystem::Load( path, data, size );

        U8 * ptr = (U8*)data;

        MeshHeader * header = (MeshHeader*)ptr;
        ptr += sizeof(MeshHeader);

        mesh.m_indexType    = toIndexType[ header->indexType ];
        mesh.m_subMeshCount = header->subMeshCount;

        VertexDeclaration& vDecl = mesh.m_vertexDecl;
        vDecl.m_count = header->inputCount;
        vDecl.m_size = 0;
        for ( SizeT i=0; i<vDecl.m_count; ++i )
        {
            MeshInput * desc = (MeshInput*)ptr;
            ptr += sizeof(MeshInput);

            vDecl.m_attributes[ i ].m_semantic      = desc->semantic;
            vDecl.m_attributes[ i ].m_type          = toAttribType[ desc->type ];
            vDecl.m_attributes[ i ].m_size          = desc->size / toAttribSize[ desc->type ];
            vDecl.m_attributes[ i ].m_normalized    = toAttribNormalized[ desc->type ];
            vDecl.m_attributes[ i ].m_offset        = desc->offset;
            vDecl.m_size                            += desc->size;
        }

        mesh.m_vertexBuffer = RenderDevice::CreateVertexBuffer( header->vertexDataSize, ptr, BU_STATIC );

        ptr += header->vertexDataSize;

        for ( SizeT i=0; i<mesh.m_subMeshCount; ++i )
        {
            SizeT count = *((U32*)ptr);
            ptr += sizeof(U32);

            SizeT size = count * DataTypeSize[ mesh.m_indexType ];

            mesh.m_subMeshes[ i ].m_indexCount   = count;
            mesh.m_subMeshes[ i ].m_indexBuffer  = RenderDevice::CreateIndexBuffer( size, ptr, BU_STATIC );
            mesh.m_subMeshes[ i ].m_vertexArray  = RenderDevice::CreateVertexArray( mesh.m_vertexDecl, mesh.m_vertexBuffer, mesh.m_subMeshes[ i ].m_indexBuffer );
            ptr += size;
        }

        UnknownAllocator::Deallocate( data );

        return mesh;
    }

    class RenderMesh : public RenderGeometry
    {
    public:
        RenderMesh( const Mesh& mesh, SizeT subMeshIndex )
            : m_mesh( mesh ), m_subMesh( mesh.m_subMeshes[ subMeshIndex ] ) {}

        void Draw()
        {
            RenderDevice::BeginGeometry( m_mesh.m_vertexDecl, m_subMesh.m_vertexArray, m_subMesh.m_indexBuffer );
            RenderDevice::DrawIndexed( m_mesh.m_primitive, m_subMesh.m_indexCount, m_mesh.m_indexType );
            RenderDevice::EndGeometry( m_mesh.m_vertexDecl );
        }

        const Mesh&     m_mesh;
        const SubMesh&  m_subMesh;
    };

    class MeshRenderer
    {
    public:
        MeshRenderer()
        {
        }

        void Initialize()
        {
            m_program                       = programCache.GetProgram( "level5" );
            m_mesh                          = LoadMesh( "sibenik.bmh" );
            m_uniformBuffers[0]             = cameraParameters;
            m_uniformBuffers[1]             = ambientParameters;
            m_uniformBuffers[2]             = lightParameters;
            m_uniformBuffers[3]             = flashParameters;

            m_renderState.m_enableDepthTest = true;
            m_renderState.m_enableCullFace  = true;
        }

        void Render()
        {
            RenderElement element;

            element.m_program           = m_program;
            element.m_renderState       = m_renderState;
            element.m_textureUnitCount  = 0;

            for ( element.m_uniformBufferCount = 0; element.m_uniformBufferCount<4; ++element.m_uniformBufferCount )
            {
                element.m_uniformBuffers[ element.m_uniformBufferCount ]  = m_uniformBuffers[ element.m_uniformBufferCount ];
            }

            for ( SizeT i=0; i<m_mesh.m_subMeshCount; ++i )
            {
                

                element.m_geometry = (RenderMesh*)MemoryManager::FrameAlloc( sizeof(RenderMesh), MemoryUtils::AlignOf< RenderMesh >() );
                ::new( element.m_geometry ) RenderMesh( m_mesh, i );

                renderList.Push( element );
            }
        }

        void Destroy()
        {
            for ( SizeT i=0; i<m_mesh.m_subMeshCount; ++i )
            {
                RenderDevice::DestroyVertexArray( m_mesh.m_subMeshes[ i ].m_vertexArray );
                RenderDevice::DestroyBuffer( m_mesh.m_subMeshes[ i ].m_indexBuffer );
            }
            RenderDevice::DestroyBuffer( m_mesh.m_vertexBuffer );
        }

    private:
        ProgramHandle   m_program;
        RenderState     m_renderState;
        Mesh            m_mesh;
        Handle          m_uniformBuffers[4];
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
        {
        }

        void Initialize()
        {
            m_program                       = programCache.GetProgram( "sphere" );
            m_mesh                          = LoadMesh( "sphere.bmh" );
            m_textureUnits[0].m_sampler     = RenderDevice::CreateSampler( FT_LINEAR, FT_LINEAR, MT_LINEAR, WT_REPEAT );
            m_textureUnits[1].m_sampler     = RenderDevice::CreateSampler( FT_LINEAR, FT_LINEAR, MT_LINEAR, WT_REPEAT );
            m_textureUnits[0].m_texture     = LoadTexture( "crack_c.btx" );
            m_textureUnits[1].m_texture     = LoadTexture( "crack_n.btx" );
            m_uniformBuffers[0]             = cameraParameters;
            m_uniformBuffers[1]             = ambientParameters;
            m_uniformBuffers[2]             = lightParameters;
            m_uniformBuffers[3]             = flashParameters;
            m_uniformBuffers[4]             = RenderDevice::CreateUniformBuffer( sizeof(SphereParameters), NULL, BU_STREAM );

            m_renderState.m_enableDepthTest = true;
            m_renderState.m_enableCullFace  = true;

            m_position                      = Vector4( 5.0f, -10.0f, 0.0f );
            m_scale                         = One4();
            m_orientation                   = Quaternion( Normalize( Vector3( 1.0f, 1.0f, -1.0f ) ), 0.0f );
        }

        void Render()
        {
            m_orientation = Quaternion( Normalize( Vector3( 1.0f, 1.0f, -1.0f ) ), 0.1f * time );

            Vector emissive = Vector4( 10.0f, 5.0f, 2.5f, 1.0f ) / SquareLength( m_position - cameraPosition );

            SphereParameters * params = static_cast< SphereParameters * >( RenderDevice::MapUniformBuffer( m_uniformBuffers[4], BA_WRITE_ONLY ) );

            params->m_world = RMatrix( m_orientation );
            Scale( params->m_world, m_scale );
            params->m_world.m_column[3] = m_position;

            params->m_emissiveColor = emissive;

            RenderDevice::UnmapUniformBuffer( );

            RenderElement element;

            element.m_program       = m_program;
            element.m_renderState   = m_renderState;

            for ( element.m_textureUnitCount = 0; element.m_textureUnitCount<2; ++element.m_textureUnitCount )
            {
                element.m_textureUnits[ element.m_textureUnitCount ] = m_textureUnits[ element.m_textureUnitCount ];
            }

            for ( element.m_uniformBufferCount = 0; element.m_uniformBufferCount<5; ++element.m_uniformBufferCount )
            {
                element.m_uniformBuffers[ element.m_uniformBufferCount ] = m_uniformBuffers[ element.m_uniformBufferCount ];
            }

            for ( SizeT i=0; i<m_mesh.m_subMeshCount; ++i )
            {
                element.m_geometry = (RenderMesh*)MemoryManager::FrameAlloc( sizeof(RenderMesh), MemoryUtils::AlignOf< RenderMesh >() );
                ::new( element.m_geometry ) RenderMesh( m_mesh, i );

                renderList.Push( element );
            }
        }

        void Destroy()
        {
            RenderDevice::DestroyBuffer( m_uniformBuffers[3] );

            RenderDevice::DestroySampler( m_textureUnits[0].m_sampler );
            RenderDevice::DestroySampler( m_textureUnits[1].m_sampler );
            RenderDevice::DestroyTexture( m_textureUnits[0].m_texture );
            RenderDevice::DestroyTexture( m_textureUnits[1].m_texture );

            for ( SizeT i=0; i<m_mesh.m_subMeshCount; ++i )
            {
                RenderDevice::DestroyVertexArray( m_mesh.m_subMeshes[ i ].m_vertexArray );
                RenderDevice::DestroyBuffer( m_mesh.m_subMeshes[ i ].m_indexBuffer );
            }
            RenderDevice::DestroyBuffer( m_mesh.m_vertexBuffer );
        }

    private:
        ProgramHandle   m_program;
        RenderState     m_renderState;
        Mesh            m_mesh;
        TextureUnit     m_textureUnits[2];
        Handle          m_uniformBuffers[5];

        Vector          m_position;
        Vector          m_scale;
        Vector          m_orientation;
    };

    LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        switch(msg)
        {
            case WM_CLOSE:
                DestroyWindow(hwnd);
            break;
            case WM_DESTROY:
                PostQuitMessage(0);
            break;
            case WM_KEYDOWN:
		    switch ( wParam )
		    {
                case VK_ESCAPE:
                    PostQuitMessage(0);
                    break;
		        case VK_F4:
                    programCache.NotifySourceChange();
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
		    break;
            case WM_KEYUP:
            switch ( wParam )
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
            break;
            case WM_INPUT:
            {
                UINT dwSize = 40;
                static BYTE lpb[40];

                GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));
                RAWINPUT* raw = (RAWINPUT*)lpb;
                if ( raw->header.dwType == RIM_TYPEMOUSE )
                {
                    mousedX = (F32)raw->data.mouse.lLastX;
                    mousedY = (F32)raw->data.mouse.lLastY;
                }
                break;
            }
            default:
                return DefWindowProc(hwnd, msg, wParam, lParam);
        }
        return 0;
    }

    Bool MessagePump( MSG * msg )
    {
        // check for messages
        while ( PeekMessage( msg, NULL, 0, 0, PM_REMOVE ) )
        {
            // handle or dispatch messages
            if ( msg->message == WM_QUIT )
            {
                return false;
            }
            else
            {
                TranslateMessage( msg );
                DispatchMessage( msg );
            }
        }

        return true;
    }

    void DisplayFramerate( HWND hwnd )
    {
        ++frameCount;
        if ( frameCount == FRAME_MAX_COUNT )
        {
            U64 currentTicks    = Core::TimeUtils::ClockTime();
            F64 fps             = FRAME_MAX_COUNT * Core::TimeUtils::ClockFrequency() / ( currentTicks - clockTicks );
            clockTicks          = currentTicks;
            frameCount          = 0;

            Char text[ 32 ];
            SetWindowText( hwnd, Core::StringUtils::FormatString( text, 32, "Level5 - [ %0.0f fps ]", fps ) );
        }
    }

    void UpdateGlobalUniformData()
    {
        U64 currentTime = Core::TimeUtils::ClockTime();
        U64 diff = currentTime - frameTime;
        frameTime = currentTime;

        time = static_cast< F32 >( frameTime * Core::TimeUtils::ClockPeriod() );
        F32 elapsedTime = static_cast< F32 >( diff * Core::TimeUtils::ClockPeriod() );
        
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

using namespace Level5_NS;

WPARAM Level5( HINSTANCE hInstance, int nCmdShow )
{
    UNIT_TEST_MESSAGE( "\n###########\n# LEVEL 5 #\n###########\n\n" );
    UNIT_TEST_MESSAGE( "Window Creation\n" );

    MSG msg;
    HWND hwnd;
    WNDCLASS wc;

    wc.style = 0;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(1 + COLOR_BTNFACE);
    wc.lpszMenuName =  NULL;
    wc.lpszClassName = "CarbonWndClass";

    if ( !RegisterClass(&wc) ) return FALSE;

    hwnd = CreateWindow( "CarbonWndClass"
                        ,"Level5"
                        ,WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU
                        ,CW_USEDEFAULT
                        ,CW_USEDEFAULT
                        ,frameWidth
                        ,frameHeight
                        ,NULL
                        ,NULL
                        ,hInstance
                        ,NULL );

    if ( !hwnd )
    {
           MessageBox( hwnd, "Cannot create the window !", "Fatal Error", MB_OK );
           return FALSE;
    }

    RECT rcClient, rcWind;
    POINT ptDiff;
    GetClientRect(hwnd, &rcClient);
    GetWindowRect(hwnd, &rcWind);
    ptDiff.x = (rcWind.right - rcWind.left) - rcClient.right;
    ptDiff.y = (rcWind.bottom - rcWind.top) - rcClient.bottom;
    MoveWindow(hwnd,rcWind.left, rcWind.top, frameWidth + ptDiff.x, frameHeight + ptDiff.y, TRUE);

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    RAWINPUTDEVICE Rid[1];
    Rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
    Rid[0].usUsage = HID_USAGE_GENERIC_MOUSE;
    Rid[0].dwFlags = RIDEV_INPUTSINK;
    Rid[0].hwndTarget = hwnd;
    RegisterRawInputDevices(Rid, 1, sizeof(Rid[0]));

    UNIT_TEST_MESSAGE( "Carbon Engine : Initialize\n" );

    MemoryManager::Initialize( frameAllocatorSize );
    FileSystem::Initialize( "..\\..\\..\\" );
    ResourceManager::Initialize();

    if ( ! device3d.Initialize( hInstance, hwnd ) )
    {
        MessageBox( hwnd, "Cannot initialize the 3D device !", "Fatal Error", MB_OK );
        return FALSE;
    }

    if ( ! programCache.Initialize( "shaders\\" ) )
    {
        device3d.Destroy();
        MessageBox( hwnd, "Cannot initialize the program cache !", "Fatal Error", MB_OK );
        return FALSE;
    }

    SharedPtr< FakeResource > test_res = ResourceManager::Create< FakeResource >( "crack_c.btx" );

    RenderCache renderCache( programCache );

    renderList.SetClearMask( CM_COLOR | CM_DEPTH );
    renderList.SetSRGBWrite( true );

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

    MeshRenderer meshRenderer;
    meshRenderer.Initialize();

    SphereRenderer sphereRenderer;
    sphereRenderer.Initialize();

    UNIT_TEST_MESSAGE( "Carbon Engine : Run\n" );

    while ( 1 )
    {
        if ( ! MessagePump( &msg ) ) break;

        /////////////////////////////////////////////////////////////////////////////

        programCache.Update();

        UpdateGlobalUniformData();

        meshRenderer.Render();
        sphereRenderer.Render();

        renderList.Draw( renderCache );

        device3d.Swap();

        MemoryManager::FrameUpdate();

        /////////////////////////////////////////////////////////////////////////////

        DisplayFramerate( hwnd );
    }

    UNIT_TEST_MESSAGE( "Carbon Engine : Destroy\n" );

    renderCache.Clear();

    RenderDevice::DestroyBuffer( flashParameters );
    RenderDevice::DestroyBuffer( lightParameters );
    RenderDevice::DestroyBuffer( ambientParameters );
    RenderDevice::DestroyBuffer( cameraParameters );

    sphereRenderer.Destroy();
    meshRenderer.Destroy();

    programCache.Destroy();

    device3d.Destroy();

    ResourceManager::Destroy();
    FileSystem::Destroy();
    MemoryManager::Destroy();

    DestroyWindow( hwnd );
    UnregisterClass( "CarbonWndClass", hInstance );

    return msg.wParam;
}
