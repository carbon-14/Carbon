#pragma once
#ifndef _UNITTEST_LEVEL7_H
#define _UNITTEST_LEVEL7_H

#include "UnitTest/Application.h"

#include "Graphic/FrameRenderer.h"

class Level7 : public Application
{
public:
    Level7( const RenderWindow& renderWindow );

protected:
    void ProcessInputs( RAWINPUT *raw );
    void PreExecute();
    void PostExecute();
    void Execute();

    Graphic::DebugRenderer              m_debugRenderer;
    Graphic::MeshRenderer               m_meshRenderer;
    Graphic::LightRenderer              m_lightRenderer;
    Graphic::FrameRenderer              m_frameRenderer;
    Graphic::FrameRenderer::Context *   m_frameContext;

    Graphic::RenderCache                m_renderCache;
};

#endif // _UNITTEST_LEVEL7_H
