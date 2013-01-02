#pragma once
#ifndef _CORE_RESOURCE_H
#define _CORE_RESOURCE_H

#include "Core/SharedPtr.h"
#include "Core/FixedString.h"

namespace Core
{
    class _CoreExport Resource : public RefCounted
    {
    public:
        enum State
        {
            INVALID,
            CREATED,
            LOADED
        };

        Resource( const Char * name );

        Bool IsValid() const;
        Bool IsLoaded() const;

        U32 GetHashCode() const;

#if defined( CARBON_DEBUG )
        const Char * GetName() const;
#endif

        virtual void Load( void * data ) = 0;

    protected:
        void SelfDelete();

        State       m_state;
        U32         m_hashCode;
#if defined( CARBON_DEBUG )
        PathString  m_name;
#endif
    };
}

#endif // _CORE_RESOURCE_H
