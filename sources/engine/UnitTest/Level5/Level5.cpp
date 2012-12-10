#include "UnitTest/Level5/Level5.h"
#include "UnitTest/Utils.h"

#include "Core/FileSystem.h"

#include "Graphic/RenderDevice.h"
#include "Graphic/ProgramCache.h"
#include "Graphic/RenderList.h"

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
    const F32 bodyWalkSpeed = 5.0f;
    const F32 bodyRunSpeed = 15.0f;
    const F32 friction = 10.0f;

    Vector bodyPosition = Vector4( 0.0f, 0.0f, 0.0f );
    Vector bodySpeed    = Zero4();
    F32 bodyMass        = 50.0f;
    F32 bodyMaxSpeed    = bodyWalkSpeed;
    Bool bodyFall       = true;
    Bool bodyJump       = false;
    F32 bodyJumpForce   = 10.0f;

    const SizeT frameAllocatorSize = 10 * 1024;

    const U32 frameWidth    = 1280;
    const U32 frameHeight   = 720;
    const F32 frameRatio    = (F32)frameWidth/(F32)frameHeight;

    RenderDevice device3d;
    ProgramCache programCache;
    RenderList renderList;
    Handle uniformBuffer;

    struct UniformData
    {
        Matrix  viewProjMatrix;
        F32 time;
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

    struct MeshInputDesc
    {
        VertexSemantic  semantic;
        VType           type;
        U32             size;
        U32             offset;
    };

    Geometry LoadMesh( const Char * filename )
    {
        Geometry geom;

        PathString path;
        FileSystem::BuildPathName( filename, path, FileSystem::PT_CACHE );

        void * data;
        SizeT size;
        FileSystem::Load( path, data, size );

        U8 * ptr = (U8*)data;

        MeshHeader * header = (MeshHeader*)ptr;
        ptr += sizeof(MeshHeader);

        geom.m_indexType    = toIndexType[ header->indexType ];
        geom.m_subGeomCount = header->subMeshCount;

        VertexDeclaration& vDecl = geom.m_vertexDecl;
        vDecl.m_count = header->inputCount;
        vDecl.m_size = 0;
        for ( SizeT i=0; i<vDecl.m_count; ++i )
        {
            MeshInputDesc * desc = (MeshInputDesc*)ptr;
            ptr += sizeof(MeshInputDesc);

            vDecl.m_attributes[ i ].m_semantic      = desc->semantic;
            vDecl.m_attributes[ i ].m_type          = toAttribType[ desc->type ];
            vDecl.m_attributes[ i ].m_size          = desc->size / toAttribSize[ desc->type ];
            vDecl.m_attributes[ i ].m_normalized    = toAttribNormalized[ desc->type ];
            vDecl.m_attributes[ i ].m_offset        = desc->offset;
            vDecl.m_size                            += desc->size;
        }

        geom.m_vertexBuffer = RenderDevice::CreateVertexBuffer( header->vertexDataSize, ptr, BU_STATIC );

        ptr += header->vertexDataSize;

        for ( SizeT i=0; i<geom.m_subGeomCount; ++i )
        {
            SizeT count = *((U32*)ptr);
            ptr += sizeof(U32);

            SizeT size = count * DataTypeSize[ geom.m_indexType ];

            geom.m_subGeoms[ i ].m_indexCount   = count;
            geom.m_subGeoms[ i ].m_indexBuffer  = RenderDevice::CreateIndexBuffer( size, ptr, BU_STATIC );
            geom.m_subGeoms[ i ].m_vertexArray  = RenderDevice::CreateVertexArray( geom.m_vertexDecl, geom.m_vertexBuffer, geom.m_subGeoms[ i ].m_indexBuffer );
            ptr += size;
        }

        UnknownAllocator::Deallocate( data );

        return geom;
    }

    class MeshRenderer
    {
    public:
        MeshRenderer()
        {
        }

        void Initialize()
        {
            m_renderElement.m_primitive = PT_TRIANGLES;
            m_renderElement.m_program   = programCache.GetProgram( "level5" );

            m_renderElement.m_geom = LoadMesh( "sibenik.bmh" );

            m_sampler = RenderDevice::CreateSampler( FT_LINEAR, FT_LINEAR, MT_LINEAR, WT_REPEAT );
            m_renderElement.m_samplers = &m_sampler;
            m_texture = LoadTexture( "carbon_c.btx" );
            m_renderElement.m_textures = &m_texture;
            m_renderElement.m_unitCount = 1;

            m_renderElement.m_uniformBuffers = &uniformBuffer;
            m_renderElement.m_uniformBufferCout = 1;
        }

        void Render()
        {
            renderList.Push( m_renderElement );
        }

        void Destroy()
        {
            RenderDevice::DestroySampler( m_sampler );
            RenderDevice::DestroyTexture( m_texture );

            Geometry& geom = m_renderElement.m_geom;
            for ( SizeT i=0; i<geom.m_subGeomCount; ++i )
            {
                RenderDevice::DestroyVertexArray( geom.m_subGeoms[ i ].m_vertexArray );
                RenderDevice::DestroyBuffer( geom.m_subGeoms[ i ].m_indexBuffer );
            }
            RenderDevice::DestroyBuffer( geom.m_vertexBuffer );
        }

    private:
        RenderElement   m_renderElement;
        Handle          m_sampler;
        Handle          m_texture;
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
		        case VK_F12:
                {
                    programCache.NotifySourceChange();
                    break;
                }
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

        F32 time = static_cast< F32 >( frameTime * Core::TimeUtils::ClockPeriod() );
        F32 elapsedTime = static_cast< F32 >( diff * Core::TimeUtils::ClockPeriod() );

        Matrix view;

        switch ( cameraMode )
        {
        case CM_Demo:
            {
                cameraPosition  = Vector4( -15.0f * Cos( time * 0.1f ), -11.5f, 1.0f );
                Vector cam_at   = Vector4( 5.0f, -10.0f, 0.0f );

                Vector cam_dir      = Normalize( cam_at - cameraPosition );
                Vector cam_up       = UnitY();
                Vector cam_right    = Normalize( Cross( cam_dir, cam_up ) );
                cam_up              = Normalize( Cross( cam_right, cam_dir ) );

                Matrix cam_base;
                cam_base.m_column[0] = cam_right;
                cam_base.m_column[1] = cam_up;
                cam_base.m_column[2] = -cam_dir;
                cam_base.m_column[3] = cameraPosition;

                view = Inverse( cam_base );

                break;
            }
        case CM_Free:
            {
                Vector rotateY = Quaternion( UnitY(), -mousedX * elapsedTime * turnSpeed );
                mousedX = 0.0f;

                Vector rotateX = Quaternion( UnitX(), -mousedY * elapsedTime * turnSpeed );
                mousedY = 0.0f;

                Vector rotate = MulQuat( rotateY, rotateX );
                cameraOrientation = MulQuat( cameraOrientation, rotateX );
                cameraOrientation = MulQuat( rotateY, cameraOrientation );

                Matrix cam_base = RMatrix( cameraOrientation );

                cameraPosition = cameraPosition - Splat(moveY * elapsedTime * moveSpeed) * cam_base.m_column[2];
                cameraPosition = cameraPosition + Splat(moveX * elapsedTime * moveSpeed) * cam_base.m_column[0];

                cam_base.m_column[3] = cameraPosition;

                view = Inverse( cam_base );

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

                Matrix cam_base = RMatrix( cameraOrientation );

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
                        Vector right = Splat( moveX ) * Select( cam_base.m_column[0], Zero4(), Mask< 0, 1, 0, 1 >() );
                        Vector front = Splat( moveY ) * Select( cam_base.m_column[2], Zero4(), Mask< 0, 1, 0, 1 >() );
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

                cam_base.m_column[3] = cameraPosition;

                view = Inverse( cam_base );

                break;
            }
        }

        F32 n       = 0.25f;
        F32 f       = 50.0f;
        F32 fov     = HalfPi();                 // need more ??? :)

        F32 cotan   = 1.0f / Tan( 0.5f * fov ) ;

        Matrix proj;
        proj.m_column[0] = Vector4( cotan   , 0.0f                  , 0.0f                          , 0.0f  );
        proj.m_column[1] = Vector4( 0.0f    , cotan * frameRatio    , 0.0f                          , 0.0f  );
        proj.m_column[2] = Vector4( 0.0f    , 0.0f                  , ( f + n ) / ( n - f )         , -1.0f );
        proj.m_column[3] = Vector4( 0.0f    , 0.0f                  , ( 2.0f * n * f ) / ( n - f )  , 0.0f  );

        UniformData  * uniformData = static_cast< UniformData * >( RenderDevice::MapUniformBuffer( uniformBuffer, BA_WRITE_ONLY ) );

        uniformData->viewProjMatrix = Mul( proj, view );
        uniformData->time           = time;

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

    if ( !hwnd ) return FALSE;

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

    if ( ! device3d.Initialize( hwnd ) )
    {
        return FALSE;
    }

    if ( ! programCache.Initialize( "shaders\\" ) )
    {
        device3d.Destroy();
        return FALSE;
    }

    renderList.SetSRGBWrite( true );

    MeshRenderer meshRenderer;
    meshRenderer.Initialize();

    uniformBuffer = RenderDevice::CreateUniformBuffer( sizeof(UniformData),NULL, BU_STREAM );

    UNIT_TEST_MESSAGE( "Carbon Engine : Run\n" );

    while ( 1 )
    {
        if ( ! MessagePump( &msg ) ) break;

        /////////////////////////////////////////////////////////////////////////////

        programCache.Update();

        UpdateGlobalUniformData();

        meshRenderer.Render();

        RenderDevice::ClearColor( 0.0f, 0.0f, 0.0f, 1.0f );

        renderList.Draw( programCache );

        device3d.Swap();

        MemoryManager::FrameUpdate();

        /////////////////////////////////////////////////////////////////////////////

        DisplayFramerate( hwnd );
    }

    UNIT_TEST_MESSAGE( "Carbon Engine : Destroy\n" );

    RenderDevice::DestroyBuffer( uniformBuffer );

    meshRenderer.Destroy();

    programCache.Destroy();

    device3d.Destroy();

    FileSystem::Destroy();
    MemoryManager::Destroy();

    DestroyWindow( hwnd );
    UnregisterClass( "CarbonWndClass", hInstance );

    return msg.wParam;
}
