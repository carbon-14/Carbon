#pragma once
#ifndef _UNITTEST_LEVEL7_H
#define _UNITTEST_LEVEL7_H

#include "UnitTest/Application.h"

#include "Graphic/RenderCache.h"
#include "Graphic/RenderList.h"

#include "Graphic/MeshResource.h"

class Level7 : public Application
{
public:
    Level7( const RenderWindow& renderWindow );

protected:
    void ProcessInputs( RAWINPUT *raw );
    void PreExecute();
    void PostExecute();
    void Execute();

    Graphic::RenderCache                m_renderCache;
    Graphic::RenderList                 m_renderList;

    SharedPtr< Graphic::MeshResource >  m_mesh;
};

#endif // _UNITTEST_LEVEL7_H
