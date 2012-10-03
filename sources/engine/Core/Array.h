#pragma once
#ifndef _CORE_ARRAY_H
#define _CORE_ARRAY_H

#include "Core/DLL.h"
#include "Core/Types.h"

#include "Core/MemoryUtils.h"
#include "Core/MemoryManager.h"
#include "Core/Algorithm.h"

#include "Core/Assert.h"

//====================================================================================
// Array
//====================================================================================

template < typename T >
class Array
{
public:
    typedef T           ValueType;
    typedef T *         Pointer;
    typedef const T *   ConstPointer;
    typedef T&          Reference;
    typedef const T&    ConstReference;
    typedef SizeT       SizeType;
    typedef T *         Iterator;
    typedef const T *   ConstIterator;

public:
    Array();
    Array( SizeType size, ConstReference value = ValueType() );
    Array( const Array& other );
    ~Array();

    void            operator=( const Array& other );

    ConstPointer    ReadData() const;
    Pointer         GetData();

    ConstIterator   Begin() const;
    Iterator        Begin();

    ConstIterator   End() const;
    Iterator        End();

    ConstReference  Front() const;
    Reference       Front();

    ConstReference  Back() const;
    Reference       Back();

    ConstReference  At( SizeType index ) const;
    Reference       At( SizeType index );

    ConstReference  operator[]( SizeType index ) const;
    Reference       operator[]( SizeType index );

    Bool            Empty() const;
    SizeType        Size() const;
    SizeType        Capacity() const;

    void            Reserve( SizeType capacity );
    void            Resize( SizeType size, ConstReference value = ValueType() );

    void            PushBackEmpty();
    void            PushBack( ConstReference value );
    void            PushBack( Iterator begin, Iterator end );

    void            PopBack();
    void            Clear();

private:
    Pointer         DoAllocate( SizeType capacity );
    void            DoFree( Pointer memory );
    SizeType        ComputeNewCapacity() const;
    void            IncreaseCapacity( SizeType capacity );

private:
    ValueType *	m_begin;
    ValueType *	m_end;
    ValueType *	m_capacity;
};

//============================================================================== Array

template< typename T >
Array< T >::Array()
: m_begin( 0 ), m_end( 0 ), m_capacity( 0 )
{
}

template< typename T >
Array< T >::Array( SizeType size, ConstReference value = ValueType() )
{
    if ( size > 0 )
    {
        m_begin	= DoAllocate( size );
        m_capacity = m_end = m_begin + size;
        Fill( m_begin, m_end, value );
    }
    else
    {
        m_begin = m_end = m_capacity = 0;
    }
}

template< typename T >
Array< T >::Array( const Array& other )
{
    const SizeType size = static_cast< SizeType >( other.m_end - other.m_begin );
    if ( size > 0 )
    {
        m_begin	= DoAllocate( size );
        m_capacity = m_end = m_begin + size;
        Copy( other.m_begin, other.m_end, m_begin );
    }
    else
    {
        m_begin = m_end = m_capacity = 0;
    }
}

template< typename T >
Array< T >::~Array()
{
    Clear();
    DoFree( m_begin );
}

template< typename T >
void Array< T >::operator=( const Array& other )
{
    Clear();
    Reserve( other.Size() );
    Copy( other.m_begin, other.m_end, m_begin );
    m_end = m_begin + other.Size();
}

template< typename T >
typename Array< T >::ConstPointer Array< T >::ReadData() const
{
    return m_begin;
}

template< typename T >
typename Array< T >::Pointer Array< T >::GetData()
{
    return m_begin;
}

template< typename T >
typename Array< T >::ConstIterator Array< T >::Begin() const
{
    return m_begin;
}

template< typename T >
typename Array< T >::Iterator Array< T >::Begin()
{
    return m_begin;
}

template< typename T >
typename Array< T >::ConstIterator Array< T >::End() const
{
    return m_end;
}

template< typename T >
typename Array< T >::Iterator Array< T >::End()
{
    return m_end;
}

template< typename T >
typename Array< T >::ConstReference Array< T >::Front() const
{
    CARBON_ASSERT( m_begin );
    return *m_begin;
}

template< typename T >
typename Array< T >::Reference Array< T >::Front()
{
    CARBON_ASSERT( m_begin );
    return *m_begin;
}

template< typename T >
typename Array< T >::ConstReference Array< T >::Back() const
{
    CARBON_ASSERT( m_begin );
    return *( m_end - 1 );
}

template< typename T >
typename Array< T >::Reference Array< T >::Back()
{
    CARBON_ASSERT( m_begin );
    return *( m_end - 1 );
}

template< typename T >
typename Array< T >::ConstReference Array< T >::At( SizeType index ) const
{
    CARBON_ASSERT( index < Size() );
    return *( m_begin + index );
}

template< typename T >
typename Array< T >::Reference Array< T >::At( SizeType index )
{
    CARBON_ASSERT( index < Size() );
    return *( m_begin + index );
}

template< typename T >
typename Array< T >::ConstReference Array< T >::operator[]( SizeType index ) const
{
    CARBON_ASSERT( index < Size() );
    return *( m_begin + index );
}

template< typename T >
typename Array< T >::Reference Array< T >::operator[]( SizeType index )
{
    CARBON_ASSERT( index < Size() );
    return *( m_begin + index );
}

template< typename T >
Bool Array< T >::Empty() const
{
    return ( m_begin == m_end );
}

template< typename T >
typename Array< T >::SizeType Array< T >::Size() const
{
    return static_cast< SizeType >( m_end - m_begin );
}

template< typename T >
typename Array< T >::SizeType Array< T >::Capacity() const
{
    return static_cast< SizeType >( m_capacity - m_begin );
}

template< typename T >
void Array< T >::Reserve( SizeType capacity )
{
    if ( Capacity() < capacity )
    {
        IncreaseCapacity( capacity );
    }
}

template< typename T >
void Array< T >::Resize( SizeType size, ConstReference value )
{
    Iterator end = m_begin + size;
    if ( size < Size() )
    {
        while ( m_end != end )
        {
            PopBack();
        }
    }
    else
    {
        Reserve( size );
        Fill( m_end, m_capacity, value );
        m_end = m_capacity;
    }
}

template< typename T >
void Array< T >::PushBackEmpty()
{
    if ( m_end == m_capacity )
    {
        IncreaseCapacity( ComputeNewCapacity() );
    }
    *m_end = ValueType();
    ++m_end;
}

template< typename T >
void Array< T >::PushBack( ConstReference value )
{
    if ( m_end == m_capacity )
    {
        IncreaseCapacity( ComputeNewCapacity() );
    }
    *m_end = value;
    ++m_end;
}

template< typename T >
void Array< T >::PushBack( Iterator begin, Iterator end )
{
    const SizeType capacity = Size() + ( end - begin );
    if ( ( m_begin + capacity ) > m_capacity )
    {
        IncreaseCapacity( capacity );
    }
    Copy( begin, end, m_end );
    m_end = m_begin + capacity;
}

template< typename T >
void Array< T >::PopBack()
{
    CARBON_ASSERT( m_begin < m_end );
    --m_end;
    m_end->~ValueType();
}

template< typename T >
void Array< T >::Clear()
{
    while ( m_begin != m_end )
    {
        PopBack();
    }
}

template< typename T >
typename Array< T >::Pointer Array< T >::DoAllocate( SizeType capacity )
{
    return static_cast< Pointer >( Core::MemoryManager::Malloc( capacity * sizeof( ValueType ), MemoryUtils::AlignOf< ValueType >() ) );
}

template< typename T >
void Array< T >::DoFree( Pointer memory )
{
    Core::MemoryManager::Free( memory );
}

template< typename T >
typename Array< T >::SizeType Array< T >::ComputeNewCapacity() const
{
    return ( m_capacity == 0 ) ? 1 : 2 * Capacity();
}

template< typename T >
void Array< T >::IncreaseCapacity( SizeType capacity )
{
    Pointer ptr = DoAllocate( capacity );
    Copy( m_begin, m_end, ptr );
    DoFree( m_begin );
    m_end = ptr + Size();
    m_begin = ptr;
    m_capacity = ptr + capacity;
}

//============================================================================== Array

//====================================================================================
// StaticArray
//====================================================================================

template < typename T, SizeT C >
class StaticArray
{
public:
    typedef T           ValueType;
    typedef T *         Pointer;
    typedef const T *   ConstPointer;
    typedef T&          Reference;
    typedef const T&    ConstReference;
    typedef SizeT       SizeType;
    typedef T *         Iterator;
    typedef const T *   ConstIterator;

public:
    StaticArray();
    StaticArray( SizeType size, ConstReference value = ValueType() );
    StaticArray( const StaticArray& other );
    ~StaticArray();

    void            operator=( const StaticArray& other );

    ConstPointer    ReadData() const;
    Pointer         GetData();

    ConstIterator   Begin() const;
    Iterator        Begin();

    ConstIterator   End() const;
    Iterator        End();

    ConstReference  Front() const;
    Reference       Front();

    ConstReference  Back() const;
    Reference       Back();

    ConstReference  At( SizeType index ) const;
    Reference       At( SizeType index );

    ConstReference  operator[]( SizeType index ) const;
    Reference       operator[]( SizeType index );

    Bool            Empty() const;
    SizeType        Size() const;
    SizeType        Capacity() const;

    void            Resize( SizeType size, ConstReference value = ValueType() );

    void            PushBackEmpty();
    void            PushBack( ConstReference value );
    void            PushBack( Iterator begin, Iterator end );

    void            PopBack();
    void            Clear();

private:
    ValueType 	m_begin[ C ];
    ValueType *	m_end;
    ValueType *	m_capacity;
};

//======================================================================== StaticArray

template< typename T, SizeT C >
StaticArray< T, C >::StaticArray()
{
    m_end = m_begin;
    m_capacity = m_begin + C;
}

template< typename T, SizeT C >
StaticArray< T, C >::StaticArray( SizeType size, ConstReference value = ValueType() )
{
    CARBON_ASSERT( size <= C );

    m_end = m_begin + size;
    m_capacity = m_begin + C;

    if ( size > 0 )
    {
        Fill( m_begin, m_end, value );
    }
}

template< typename T, SizeT C >
StaticArray< T, C >::StaticArray( const StaticArray& other )
{
    m_capacity = m_begin + C;

    const SizeType size = static_cast< SizeType >( other.m_end - other.m_begin );
    if ( size > 0 )
    {
        Copy( (Iterator)other.m_begin, (Iterator)other.m_end, m_begin );
    }
    else
    {
        m_end = m_begin;
    }
}

template< typename T, SizeT C >
StaticArray< T, C >::~StaticArray()
{
    Clear();
}

template< typename T, SizeT C >
void StaticArray< T, C >::operator=( const StaticArray& other )
{
    Copy( (Iterator)other.m_begin, (Iterator)other.m_end, m_begin );
    m_end = m_begin + other.Size();
}

template< typename T, SizeT C >
typename StaticArray< T, C >::ConstPointer StaticArray< T, C >::ReadData() const
{
    return m_begin;
}

template< typename T, SizeT C >
typename StaticArray< T, C >::Pointer StaticArray< T, C >::GetData()
{
    return m_begin;
}

template< typename T, SizeT C >
typename StaticArray< T, C >::ConstIterator StaticArray< T, C >::Begin() const
{
    return m_begin;
}

template< typename T, SizeT C >
typename StaticArray< T, C >::Iterator StaticArray< T, C >::Begin()
{
    return m_begin;
}

template< typename T, SizeT C >
typename StaticArray< T, C >::ConstIterator StaticArray< T, C >::End() const
{
    return m_end;
}

template< typename T, SizeT C >
typename StaticArray< T, C >::Iterator StaticArray< T, C >::End()
{
    return m_end;
}

template< typename T, SizeT C >
typename StaticArray< T, C >::ConstReference StaticArray< T, C >::Front() const
{
    CARBON_ASSERT( m_begin != m_end );
    return *m_begin;
}

template< typename T, SizeT C >
typename StaticArray< T, C >::Reference StaticArray< T, C >::Front()
{
    CARBON_ASSERT( m_begin != m_end );
    return *m_begin;
}

template< typename T, SizeT C >
typename StaticArray< T, C >::ConstReference StaticArray< T, C >::Back() const
{
    CARBON_ASSERT( m_begin != m_end );
    return *( m_end - 1 );
}

template< typename T, SizeT C >
typename StaticArray< T, C >::Reference StaticArray< T, C >::Back()
{
    CARBON_ASSERT( m_begin != m_end );
    return *( m_end - 1 );
}

template< typename T, SizeT C >
typename StaticArray< T, C >::ConstReference StaticArray< T, C >::At( SizeType index ) const
{
    CARBON_ASSERT( index < Size() );
    return *( m_begin + index );
}

template< typename T, SizeT C >
typename StaticArray< T, C >::Reference StaticArray< T, C >::At( SizeType index )
{
    CARBON_ASSERT( index < Size() );
    return *( m_begin + index );
}

template< typename T, SizeT C >
typename StaticArray< T, C >::ConstReference StaticArray< T, C >::operator[]( SizeType index ) const
{
    CARBON_ASSERT( index < Size() );
    return *( m_begin + index );
}

template< typename T, SizeT C >
typename StaticArray< T, C >::Reference StaticArray< T, C >::operator[]( SizeType index )
{
    CARBON_ASSERT( index < Size() );
    return *( m_begin + index );
}

template< typename T, SizeT C >
Bool StaticArray< T, C >::Empty() const
{
    return ( m_begin == m_end );
}

template< typename T, SizeT C >
typename StaticArray< T, C >::SizeType StaticArray< T, C >::Size() const
{
    return static_cast< SizeType >( m_end - m_begin );
}

template< typename T, SizeT C >
typename StaticArray< T, C >::SizeType StaticArray< T, C >::Capacity() const
{
    return static_cast< SizeType >( C );
}

template< typename T, SizeT C >
void StaticArray< T, C >::Resize( SizeType size, ConstReference value )
{
    CARBON_ASSERT( size <= C );

    Iterator end = m_begin + size;
    if ( size < Size() )
    {
        while ( m_end != end )
        {
            PopBack();
        }
    }
    else
    {
        Fill( m_end, end, value );
        m_end = end;
    }
}

template< typename T, SizeT C >
void StaticArray< T, C >::PushBackEmpty()
{
    CARBON_ASSERT( m_end < m_capacity );

    *m_end = ValueType();
    ++m_end;
}

template< typename T, SizeT C >
void StaticArray< T, C >::PushBack( ConstReference value )
{
    CARBON_ASSERT( m_end < m_capacity );

    *m_end = value;
    ++m_end;
}

template< typename T, SizeT C >
void StaticArray< T, C >::PushBack( Iterator begin, Iterator end )
{
    const SizeType capacity = Size() + ( end - begin );

    CARBON_ASSERT( capacity <= C );

    Copy( begin, end, m_end );
    m_end = m_begin + capacity;
}

template< typename T, SizeT C >
void StaticArray< T, C >::PopBack()
{
    CARBON_ASSERT( m_begin < m_end );
    --m_end;
    m_end->~ValueType();
}

template< typename T, SizeT C >
void StaticArray< T, C >::Clear()
{
    while ( m_begin != m_end )
    {
        PopBack();
    }
}

//======================================================================== StaticArray

#endif // _CORE_ARRAY_H
