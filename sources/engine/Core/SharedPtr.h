#pragma once
#ifndef _CORE_SHAREDPTR_H
#define _CORE_SHAREDPTR_H

#include "Core/DLL.h"
#include "Core/Types.h"

#include "Core/Assert.h"

//====================================================================================
// RefCounted
//====================================================================================

class _CoreExport RefCounted
{
public:
    RefCounted();

    void Increment();
    void Decrement();

    SizeT GetRefCount() const;

protected:
    virtual void SelfDelete() = 0;

private:
    RefCounted( const RefCounted& );

private:
    SizeT   m_refCount;
};

//========================================================================= RefCounted

//====================================================================================
// SharedPtr
//====================================================================================

template< typename T >
class SharedPtr
{
public:
    SharedPtr();
    SharedPtr( T * ptr );
    SharedPtr( SharedPtr& ptr );
    ~SharedPtr();

    SharedPtr& operator=( T * ptr );
    SharedPtr& operator=( SharedPtr& ptr );

    const T * ConstPtr() const;
    T * Ptr();

    const T * operator->() const;
    T * operator->();

    const T& operator*() const;
    T& operator*();

    operator Bool() const;

private :
    void Release();
    void Lock();

    T * m_ptr;
};

//========================================================================== SharedPtr

template< typename T >
SharedPtr< T >::SharedPtr()
    : m_ptr( 0 )
{
}

template< typename T >
SharedPtr< T >::SharedPtr( T * ptr )
{
    m_ptr = ptr;
    Lock();
}

template< typename T >
SharedPtr< T >::SharedPtr( SharedPtr& ptr )
{
    m_ptr = ptr.Ptr();
    Lock();
}

template< typename T >
SharedPtr< T >::~SharedPtr()
{
    Release();
}

template< typename T >
SharedPtr< T >& SharedPtr< T >::operator=( T * ptr )
{
    if ( ptr != m_ptr )
    {
        Release();
        m_ptr = ptr;
        Lock();
    }

    return *this;
}

template< typename T >
SharedPtr< T >& SharedPtr< T >::operator=( SharedPtr& ptr )
{
    if ( ptr.Ptr() != m_ptr )
    {
        Release();
        m_ptr = ptr.Ptr();
        Lock();
    }

    return *this;
}

template< typename T >
const T * SharedPtr< T >::ConstPtr() const
{
    return m_ptr;
}

template< typename T >
T * SharedPtr< T >::Ptr()
{
    return m_ptr;
}

template< typename T >
const T * SharedPtr< T >::operator->() const
{
    return m_ptr;
}

template< typename T >
T * SharedPtr< T >::operator->()
{
    return m_ptr;
}

template< typename T >
const T& SharedPtr< T >::operator*() const
{
    CARBON_ASSERT( m_ptr );
    return *m_ptr;
}

template< typename T >
T& SharedPtr< T >::operator*()
{
    CARBON_ASSERT( m_ptr );
    return *m_ptr;
}

template< typename T >
SharedPtr< T >::operator Bool() const
{
    return m_ptr != 0;
}

template< typename T >
void SharedPtr< T >::Lock()
{
    if ( m_ptr )
    {
        m_ptr->Increment();
    }
}

template< typename T >
void SharedPtr< T >::Release()
{
    if ( m_ptr )
    {
        m_ptr->Decrement();
    }
}

//========================================================================== SharedPtr

#endif // _CORE_SHAREDPTR_H
