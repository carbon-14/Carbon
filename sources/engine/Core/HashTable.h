#pragma once
#ifndef _CORE_HASHTABLE_H
#define _CORE_HASHTABLE_H

#include "Core/Bucket.h"

namespace Core
{
    //====================================================================================
    // HashTable
    //====================================================================================

    template< typename T, typename K = U32, SizeT S = 256, typename Alloc = DefaultAllocator >
    class HashTable
    {
    public:
        typedef K           KeyType;
        typedef T           ValueType;
        typedef T *         Pointer;
        typedef const T *   ConstPointer;
        typedef T&          Reference;
        typedef const T&    ConstReference;
        typedef SizeT       SizeType;

        struct Pair
        {
            KeyType         m_key;
            ValueType       m_value;
        };

        typedef Bucket< Pair, Alloc >  BucketType;

    public:
        HashTable();

        SizeType            Count() const;

        ConstPointer        Find( KeyType key ) const;
        Pointer             Find( KeyType key );

        void                Insert( KeyType key, ConstReference value );
        void                Remove( KeyType key );

        void                Clear();

        template< typename ArrayAlloc >
        void                Dump( Array< ValueType, ArrayAlloc >& dmp )
        {
            BucketType * b = m_table;
            BucketType * bEnd = m_table + S;
            for ( ; b != bEnd; ++b )
            {
                BucketType::Iterator p = b->Begin();
                BucketType::ConstIterator pEnd = b->End();
                for ( ; p != pEnd; ++p )
                {
                    dmp.PushBack( p->m_value );
                }
            }
        }

    private:
        BucketType          m_table[ S ];
    };

    //========================================================================== HashTable

    template< typename T, typename K, SizeT S, typename Alloc >
    HashTable< T, K, S, Alloc >::HashTable()
    {

    }

    template< typename T, typename K, SizeT S, typename Alloc >
    typename HashTable< T, K, S, Alloc >::SizeType HashTable< T, K, S, Alloc >::Count() const
    {
        SizeType count = 0;

        const BucketType * b = m_table;
        const BucketType * end = m_table + S;
        for ( ; b != end; ++b )
        {
            count += b->Size();
        }

        return count;
    }

    template< typename T, typename K, SizeT S, typename Alloc >
    void HashTable< T, K, S, Alloc >::Insert( KeyType key, const ValueType& value )
    {
        SizeType idx = key % S;

        Pair e = { key, value };
        m_table[idx].PushBack( e );
    }

    template< typename T, typename K, SizeT S, typename Alloc >
    void HashTable< T, K, S, Alloc >::Remove( KeyType key )
    {
        SizeType idx = key % S;

        BucketType::Iterator it = m_table[idx].Begin();
        BucketType::ConstIterator end = m_table[idx].End();
        for ( ; it != end; ++it )
        {
            if ( key == it->m_key )
            {
                m_table[idx].Remove( it );
                break;
            }
        }
    }

    template< typename T, typename K, SizeT S, typename Alloc >
    typename HashTable< T, K, S, Alloc >::ConstPointer HashTable< T, K, S, Alloc >::Find( KeyType key ) const
    {
        SizeT idx = key % S;

        BucketType::ConstIterator it = m_table[idx].Begin();
        BucketType::ConstIterator end = m_table[idx].End();
        for ( ; it != end; ++it )
        {
            if ( key == it->m_key )
            {
                value = &(it->m_value);
            }
        }

        return 0;
    }

    template< typename T, typename K, SizeT S, typename Alloc >
    typename HashTable< T, K, S, Alloc >::Pointer HashTable< T, K, S, Alloc >::Find( KeyType key )
    {
        SizeT idx = key % S;

        BucketType::Iterator it = m_table[idx].Begin();
        BucketType::ConstIterator end = m_table[idx].End();
        for ( ; it != end; ++it )
        {
            if ( key == it->m_key )
            {
                return &(it->m_value);
            }
        }

        return 0;
    }

    template< typename T, typename K, SizeT S, typename Alloc >
    void HashTable< T, K, S, Alloc >::Clear()
    {
        BucketType * b = m_table;
        BucketType * end = m_table + S;
        for ( ; b != end; ++b )
        {
            b->Clear();
        }
    }

    //========================================================================== HashTable
}

#endif // _CORE_HASHTABLE_H
