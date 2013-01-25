#pragma once
#ifndef _UNITTEST_LEVEL7_H
#define _UNITTEST_LEVEL7_H

#include "UnitTest/Application.h"

class Level7 : public Application
{
public:
    Level7( const RenderWindow& renderWindow );

protected:
    void ProcessInputs( RAWINPUT *raw );
    void PreExecute();
    void PostExecute();
    void Execute();
};

#endif // _UNITTEST_LEVEL7_H
