#include "UnitTest/Level7/Level7.h"

using namespace Core;
using namespace Graphic;

namespace Level7_NS
{
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
}

void Level7::PostExecute()
{
}

void Level7::Execute()
{
    Handle depthStencil = RenderDevice::CreateRenderTarget( TF_D24S8, m_window.width, m_window.height );
    Handle albedo = RenderDevice::CreateRenderTarget( TF_RGBA8, m_window.width, m_window.height );

    Handle framebuffer = RenderDevice::CreateFramebuffer();

    RenderDevice::BindFramebuffer( framebuffer, FT_DRAW );
    RenderDevice::AttachTexture( FT_DRAW, FA_COLOR0, albedo, 0 );
    RenderDevice::AttachTexture( FT_DRAW, FA_DEPTH_STENCIL, depthStencil, 0 );

    RenderDevice::AttachTexture( FT_DRAW, FA_DEPTH_STENCIL, 0, 0 );
    RenderDevice::AttachTexture( FT_DRAW, FA_COLOR0, 0, 0 );
    RenderDevice::BindFramebuffer( 0, FT_DRAW );

    RenderDevice::DestroyTexture( albedo );
    RenderDevice::DestroyTexture( depthStencil );
}

