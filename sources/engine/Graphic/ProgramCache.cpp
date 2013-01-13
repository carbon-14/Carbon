#include "Graphic/ProgramCache.h"

#include "Core/FileSystem.h"
#include "Core/Hash.h"
#include "Core/Trace.h"

using namespace Core;

namespace Graphic
{
    enum ShaderTypeBits
    {
        STB_VERTEX_SHADER    = 1 << ST_VERTEX_SHADER,
        STB_FRAGMENT_SHADER  = 1 << ST_FRAGMENT_SHADER,
        STB_GEOMETRY_SHADER  = 1 << ST_GEOMETRY_SHADER
    };

    const Char Extensions[ ST_COUNT ][ 4 ] = { ".vs", ".fs", ".gs" };

    const char samplersFileName[] = "samplers.bin";

    struct SamplerDesc
    {
        unsigned char m_min     : 1;
        unsigned char m_mag     : 1;
        unsigned char m_mip     : 2;
        unsigned char m_wrap    : 2;
    };

    const SizeT maxProgramSetCount = 16;

    Bool dirtyCache = false;

    const ProgramHandle ProgramCache::ms_invalidHandle = -1;

    Bool ProgramCache::Initialize( const Char * shaderPath, const Char * materialPath )
    {
        CARBON_ASSERT( m_dataPath.Empty() );
        CARBON_ASSERT( m_cachePath.Empty() );
        CARBON_ASSERT( m_materialPath.Empty() );

        FileSystem::BuildPathName( shaderPath, m_dataPath, FileSystem::PT_DATA );
        FileSystem::BuildPathName( shaderPath, m_cachePath, FileSystem::PT_CACHE );
        FileSystem::BuildPathName( materialPath, m_materialPath, FileSystem::PT_CACHE );

        BuildCache();

        m_programCache = 0;
        memset( &m_samplerCache, -1, sizeof(m_samplerCache) );
        m_samplerCacheCount = 0;
        m_uniformBufferCache = 0;

        return true;
    }

    void ProgramCache::Destroy()
    {
        SetProgram( 0 );
        SetSamplers( NULL, 0 );
        SetUniformBuffer( 0 );

        m_dataPath.Clear();
        m_cachePath.Clear();
        m_materialPath.Clear();

        // Clear samplers
        {
            SamplerArray::Iterator it   = m_samplers.Begin();
            SamplerArray::Iterator end  = m_samplers.End();
            for ( ; it != end; ++it )
            {
                RenderDevice::DestroySampler( *it );
            }
            m_samplers.Clear();
        }
        // Clear programs
        {
            ProgramArray::Iterator it   = m_programs.Begin();
            ProgramArray::Iterator end  = m_programs.End();
            for ( ; it != end; ++it )
            {
                RenderDevice::DeleteProgram( it->m_handle );
            }
            m_programs.Clear();
        }
        // Clear program sets
        {
            ProgramSetArray::Iterator it   = m_programSets.Begin();
            ProgramSetArray::Iterator end  = m_programSets.End();
            for ( ; it != end; ++it )
            {
                RenderDevice::DestroyBuffer( it->m_uniformBuffer );
            }
            m_programSets.Clear();
        }
    }

    void ProgramCache::Update()
    {
        if ( dirtyCache )
        {
            ReloadCache();
            dirtyCache = false;
        }
    }

    ProgramHandle ProgramCache::GetProgram( const Char * name, const Char * set ) const
    {
        CARBON_ASSERT( name );
        CARBON_ASSERT( set );

        return GetProgram( CreateId( name ), CreateId( set ) );
    }

    ProgramHandle ProgramCache::GetProgram( U32 nameId, U32 setId ) const
    {
        ProgramArray::ConstIterator prg = Find( nameId );

        ProgramHandle handle = ( prg - m_programs.Begin() ) << 4;

        if ( setId == CreateId( "" ) )
            return handle;

        ProgramSetArray::ConstIterator it = m_programSets.Begin() + handle;
        ProgramSetArray::ConstIterator end = it + prg->m_setCount;
        for ( ; it != end && it->m_id != setId; ++it );

        return it - m_programSets.Begin();
    };

    void ProgramCache::UseProgram( ProgramHandle handle )
    {
        CARBON_ASSERT( handle != ms_invalidHandle );

        const Program& p = m_programs[ handle >> 4 ];

        SetProgram( p.m_handle );
        SetSamplers( p.m_samplers, p.m_samplerCount );
        SetUniformBuffer( m_programSets[ handle ].m_uniformBuffer );
    }

    void ProgramCache::NotifySourceChange()
    {
        dirtyCache = true;
    }

    void ProgramCache::BuildCache()
    {
        LoadSamplerList();

        PathString searchStr = m_dataPath;
        searchStr += "*";

        Array< PathString > shaderFileNames;
        FileSystem::Find( searchStr.ConstPtr(), shaderFileNames, false );

        // Collect infos
        Array< PathString >::Iterator sIt = shaderFileNames.Begin();
        Array< PathString >::Iterator sEnd = shaderFileNames.End();
        for ( ; sIt != sEnd; ++sIt )
        {
            PathString& name = *sIt;

            CARBON_ASSERT( name.Size() > 3 );

            const Char * ext = name.End() - 3;

            U32 typeMask = 0;
            if ( StringUtils::StrCmp( ext, ".vs" ) == 0 )
            {
                typeMask |= STB_VERTEX_SHADER;
            }
            else if ( StringUtils::StrCmp( ext, ".fs" ) == 0 )
            {
                typeMask |= STB_FRAGMENT_SHADER;
            }
            else if ( StringUtils::StrCmp( ext, ".gs" ) == 0 )
            {
                typeMask |= STB_GEOMETRY_SHADER;
            }
            else
            {
                CARBON_ASSERT( !"shader extension is not managed" );
                continue;
            }

            name.Resize( ext - name.Begin() );
            *(name.End()) = 0;

            U32 id = CreateId( name.ConstPtr() );
            ProgramArray::Iterator program = Find( id );

            if ( program == m_programs.End() )
            {
                m_programs.PushBack( Program( id, typeMask, name.ConstPtr() ) );
                program = m_programs.End() - 1;
            }

            program->m_type |= typeMask;
        }

        ProgramArray::Iterator pIt   = m_programs.Begin();
        ProgramArray::Iterator pEnd  = m_programs.End();
        for ( ; pIt != pEnd; ++pIt )
        {
            Program& program = *pIt;
            LoadProgram( program );
        }

        LoadProgramSets();
    }

    void ProgramCache::ReloadCache()
    {
        // Reload samplers
        {
            SamplerArray::Iterator it   = m_samplers.Begin();
            SamplerArray::Iterator end  = m_samplers.End();
            for ( ; it != end; ++it )
            {
                RenderDevice::DestroySampler( *it );
            }
            m_samplers.Clear();
            LoadSamplerList();
        }
        // Reload programs
        {
            ProgramArray::Iterator it   = m_programs.Begin();
            ProgramArray::Iterator end  = m_programs.End();
            for ( ; it != end; ++it )
            {
                Program& program = *it;
                RenderDevice::DeleteProgram( program.m_handle );
                program.m_handle = 0;
                LoadProgram( program );
            }
        }
        // Reload program sets
        {
            ProgramSetArray::Iterator it   = m_programSets.Begin();
            ProgramSetArray::Iterator end  = m_programSets.End();
            for ( ; it != end; ++it )
            {
                RenderDevice::DestroyBuffer( it->m_uniformBuffer );
            }
            m_programSets.Clear();
            LoadProgramSets();
        }
    }

    void ProgramCache::LoadSamplerList()
    {
        PathString fileName = m_materialPath;
        fileName += samplersFileName;

        void * buffer;
        SizeT size;

        if ( FileSystem::Load( fileName, buffer, size ) )
        {
            U8 * ptr = (U8*)buffer;

            SizeT count     = *((U32*)ptr);
            ptr            += sizeof(U32);

            m_samplers.Reserve( count );
            while ( count )
            {
                SamplerDesc desc    = *((SamplerDesc*)ptr);
                ptr                 += sizeof(SamplerDesc);

                m_samplers.PushBack( RenderDevice::CreateSampler( (FilterType)desc.m_min, (FilterType)desc.m_mag, (MipType)desc.m_mip, (WrapType)desc.m_wrap ) );
                --count;
            }

            UnknownAllocator::Deallocate( buffer );
        }
    }

    void ProgramCache::LoadProgram( Program& program )
    {
        // Try to load program from cache file
        if ( ! program.m_handle )
        {
            LoadProgramFromBinaries( program );
        }

        // Load data from source files
        if ( ! program.m_handle )
        {
            LoadProgramFromSources( program );
        }
    }

    void ProgramCache::LoadProgramFromSources( Program& program )
    {
        void * srcBuffers[ ST_COUNT ];
        SizeT srcSizes[ ST_COUNT ];
        ShaderType srcTypes[ ST_COUNT ];

        SizeT count = 0;
        for ( SizeT i=0; i<ST_COUNT; ++i )
        {
            U32 mask = 1 << i;
            if ( program.m_type & mask )
            {
                PathString fileName = m_dataPath;
                fileName += program.m_name;
                fileName += Extensions[ i ];

                if ( FileSystem::Load( fileName, srcBuffers[ count ], srcSizes[ count ]  ) )
                {
                    srcTypes[ count ]   = (ShaderType)i;

                    CARBON_ASSERT( srcSizes[ count ] > 0 );
                    reinterpret_cast< Char * >(srcBuffers[ count ])[ srcSizes[ count ] - 1 ] = 0;    // last character used as eof

                    ++count;
                }
            }
        }

        program.m_handle = RenderDevice::CreateProgram( (const Char **)srcBuffers, srcSizes, srcTypes, count );

        for ( SizeT i=0; i<count; ++i )
        {
            UnknownAllocator::Deallocate( srcBuffers[ i ] );
        }

        if ( program.m_handle )
        {
            PathString fileName = m_cachePath;
            fileName += program.m_name;
            fileName += ".bin";

            SizeT size;
            void * buffer;
            RenderDevice::GetProgramBinary( program.m_handle, buffer, size );

            FileSystem::Save( fileName, buffer, size );
        }
    }

    void ProgramCache::LoadProgramFromBinaries( Program& program )
    {
        PathString binFileName = m_cachePath;
        binFileName += program.m_name;
        binFileName += ".bin";

        if ( FileSystem::Exists( binFileName ) )
        {
            PathString searchStr = m_dataPath;
            searchStr += program.m_name;
            searchStr += ".*";

            Array< PathString > shaderFileNames;
            FileSystem::Find( searchStr.ConstPtr(), shaderFileNames );

            U64 binLastWriteTime = FileSystem::GetLastWriteTime( binFileName );

            Bool binIsUpToDate = true;

            Array< PathString >::ConstIterator it = shaderFileNames.Begin();
            Array< PathString >::ConstIterator end = shaderFileNames.End();
            for ( ; it != end; ++it )
            {
                const PathString& shaderFileName = *it;

                if ( binLastWriteTime < FileSystem::GetLastWriteTime( shaderFileName ) )
                {
                    binIsUpToDate = false;
                    break;
                }
            }

            if ( binIsUpToDate )
            {
                SizeT size;
                void * buffer;
                if ( FileSystem::Load( binFileName, buffer, size ) )
                {
                    program.m_handle = RenderDevice::CreateProgramBinary( buffer, size ); 
                    UnknownAllocator::Deallocate( buffer );
                }
            }
        }
    }

    void ProgramCache::LoadProgramSets()
    {
        m_programSets.Resize( m_programs.Size() * maxProgramSetCount );

        for ( SizeT i=0; i<m_programs.Size(); ++i )
        {
            Program& program = m_programs[i];

            PathString fileName = m_materialPath;
            fileName += program.m_name;
            fileName += ".bin";

            SizeT handle = i << 4;

            void * buffer;
            SizeT size;

            CARBON_ASSERT( FileSystem::Load( fileName, buffer, size ) );

            U8 * ptr = (U8*)buffer;

            SizeT samplerCount  = *((U32*)ptr);
            ptr                 += sizeof(U32);

            for ( program.m_samplerCount=0; program.m_samplerCount<samplerCount; ++program.m_samplerCount )
            {
                program.m_samplers[ program.m_samplerCount ]    = *((U32*)ptr);
                ptr                                             += sizeof(U32);
            }

            SizeT setSize   = *((U32*)ptr);
            ptr             += sizeof(U32);

            SizeT setCount  = *((U32*)ptr);
            ptr             += sizeof(U32);

            for ( program.m_setCount=0; program.m_setCount<setCount; ++program.m_setCount )
            {
                ProgramSet& set = m_programSets[ handle + program.m_setCount ];

                set.m_id    = *((U32*)ptr);
                ptr         += sizeof(U32);

                if ( setSize )
                {
                    set.m_uniformBuffer = RenderDevice::CreateUniformBuffer( setSize, ptr, BU_STATIC );
                    ptr                 += setSize;
                }
                else
                {
                    set.m_uniformBuffer = 0;
                }
            }

            UnknownAllocator::Deallocate( buffer );
        }
    }

    U32 ProgramCache::CreateId( const Char * str )
    {
        return Core::HashString( str );
    }

    ProgramCache::ProgramArray::ConstIterator ProgramCache::Find( U32 id ) const
    {
        ProgramArray::ConstIterator it  = m_programs.Begin();
        ProgramArray::ConstIterator end = m_programs.End();
        for ( ; it != end && it->m_id != id ; ++it );

        return it;
    }

    ProgramCache::ProgramArray::Iterator ProgramCache::Find( U32 id )
    {
        ProgramArray::Iterator it  = m_programs.Begin();
        ProgramArray::Iterator end = m_programs.End();
        for ( ; it != end && it->m_id != id ; ++it );

        return it;
    }

    void ProgramCache::SetProgram( Handle program )
    {
        if ( program != m_programCache )
        {
            RenderDevice::UseProgram( program );
            m_programCache = program;
        }
    }

    void ProgramCache::SetSamplers( const SizeT * samplers, SizeT count )
    {
        if ( count <= m_samplerCacheCount )
        {
            for ( SizeT i=0; i<count; ++i )
            {
                if ( samplers[i] != m_samplerCache[i] )
                {
                    RenderDevice::BindSampler( m_samplers[ samplers[i] ], i );
                    m_samplerCache[i] = samplers[ i ];
                }
            }
        }
        else
        {
            for ( SizeT i=0; i<m_samplerCacheCount; ++i )
            {
                if ( samplers[i] != m_samplerCache[i] )
                {
                    RenderDevice::BindSampler( m_samplers[ samplers[i] ], i );
                    m_samplerCache[i] = samplers[ i ];
                }
            }

            for ( SizeT i=m_samplerCacheCount; i<count; ++i )
            {
                RenderDevice::BindSampler( m_samplers[ samplers[i] ], i );
                m_samplerCache[i] = samplers[ i ];
            }
        }

        m_samplerCacheCount = count;
    }

    void ProgramCache::SetUniformBuffer( Handle uniformBuffer )
    {
        if ( uniformBuffer != m_uniformBufferCache )
        {
            RenderDevice::BindUniformBuffer( uniformBuffer, RenderDevice::ms_maxUniformBufferCount );
            m_uniformBufferCache = uniformBuffer;
        }
    }
}
