#pragma once
#ifndef _CORE_RESOURCE_H
#define _CORE_RESOURCE_H

#include "Core/SharedPtr.h"
#include "Core/FixedString.h"

class _CoreExport Resource : public RefCounted
{
    friend class ResourceManager;

public:
    Resource();
    virtual ~Resource();

    bool            IsValid() const;    // valid
    bool            IsPending() const;  // pending
    bool            IsLoaded() const;   // loaded
    bool            IsReady() const;    // valid & loaded

    U32             GetId() const;

#if defined( CARBON_DEBUG )
    const Char *    GetName() const;
#endif

    static U32      MakeIdFromName( const Char * name );

protected:
    enum StateBit
    {
        VALID   = 1 << 0,
        PENDING = 1 << 1,
        LOADED  = 1 << 2
    };

    void            SetId( U32 id );
#if defined( CARBON_DEBUG )
    void            SetName( const Char * name );
#endif

    virtual bool    Load( const void * data ) = 0;
    virtual void    Unload() = 0;

    void            SelfDelete();

    U32             m_state;
    U32             m_id;

#if defined( CARBON_DEBUG )
    PathString      m_name;
#endif
};

#endif // _CORE_RESOURCE_H
