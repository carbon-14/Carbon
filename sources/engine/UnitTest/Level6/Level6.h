#pragma once
#ifndef _UNITTEST_LEVEL6_H
#define _UNITTEST_LEVEL6_H

#include "UnitTest/Application.h"

#include "Graphic/RenderCache.h"
#include "Graphic/RenderList.h"

#include "Core/Vector.h"

class Level6 : public Application
{
public:
    Level6( const RenderWindow& renderWindow );

protected:
    void ProcessInputs( RAWINPUT *raw );
    void PreExecute();
    void PostExecute();
    void Execute();

    Graphic::RenderCache    m_renderCache;
    Graphic::RenderList     m_renderList;
};

#endif // _UNITTEST_LEVEL6_H
