#pragma once
#ifndef _CORE_RESOURCE_H
#define _CORE_RESOURCE_H

#include "Core/SharedPtr.h"
#include "Core/FixedString.h"

namespace Core
{
    class _CoreExport Resource : public RefCounted
    {
        friend class ResourceManager;

    public:
        enum State
        {
            INVALID,
            CREATED,
            LOADED
        };

        Resource();
        virtual ~Resource();

        State           GetState() const;
        Bool            IsValid() const;
        Bool            IsLoaded() const;

        U32             GetId() const;

#if defined( CARBON_DEBUG )
        const Char *    GetName() const;
#endif

        static U32      MakeIdFromName( const Char * name );

    protected:
        void            SetId( U32 id );
#if defined( CARBON_DEBUG )
        void            SetName( const Char * name );
#endif

        virtual void    Load( const void * data ) = 0;

        void            SelfDelete();

        State           m_state;
        U32             m_id;

#if defined( CARBON_DEBUG )
        PathString      m_name;
#endif
    };
}

#endif // _CORE_RESOURCE_H
