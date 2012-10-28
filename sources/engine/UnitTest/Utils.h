#pragma once
#ifndef _UNITTEST_UTILS_H
#define _UNITTEST_UTILS_H

#include "Core/StringUtils.h"
#include "Core/Trace.h"

#define UNIT_TEST_DEBUG_STRING_SIZE 512

#define UNIT_TEST_MESSAGE( format, ... ) \
    { \
        Char unit_test_dbgString[ UNIT_TEST_DEBUG_STRING_SIZE ]; \
        Core::StringUtils::FormatString( unit_test_dbgString, UNIT_TEST_DEBUG_STRING_SIZE, format, __VA_ARGS__ ); \
        Core::Trace::Message( unit_test_dbgString ); \
    }

#endif // _UNITTEST_UTILS_H
