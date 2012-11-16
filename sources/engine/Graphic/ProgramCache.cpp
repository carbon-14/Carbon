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

    Bool dirtyCache = false;

    const ProgramHandle ProgramCache::ms_invalidHandle = -1;

    Bool ProgramCache::Initialize( const Char * relativePath )
    {
        CARBON_ASSERT( m_dataPath.Empty() );
        CARBON_ASSERT( m_cachePath.Empty() );

        FileSystem::BuildPathName( relativePath, m_dataPath, FileSystem::PT_DATA );
        FileSystem::BuildPathName( relativePath, m_cachePath, FileSystem::PT_CACHE );

        BuildCache();

        return true;
    }

    void ProgramCache::Destroy()
    {
        m_dataPath.Clear();
        m_cachePath.Clear();
        m_cache.Clear();
    }

    void ProgramCache::Update()
    {
        if ( dirtyCache )
        {
            ReloadCache();
            dirtyCache = false;
        }
    }

    ProgramHandle ProgramCache::GetProgram( const Char * name ) const
    {
        U32 id = CreateId( name );

        return FindById( id );
    }

    void ProgramCache::UseProgram( ProgramHandle handle ) const
    {
        CARBON_ASSERT( handle != ms_invalidHandle );

        const Program& p = m_cache[ handle ];
        RenderDevice::UseProgram( p.m_handle );
    }

    void ProgramCache::NotifySourceChange()
    {
        dirtyCache = true;
    }

    void ProgramCache::BuildCache()
    {
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
            SizeT index = FindById( id );

            Program * program = 0;
            if ( index == ms_invalidHandle )
            {
                m_cache.PushBack( Program( id, typeMask, name.ConstPtr() ) );
                program = m_cache.End() - 1;
            }
            else
            {
                program = &m_cache[ index ];
            }

            program->m_type |= typeMask;
        }

        ProgramArray::Iterator pIt   = m_cache.Begin();
        ProgramArray::Iterator pEnd  = m_cache.End();
        for ( ; pIt != pEnd; ++pIt )
        {
            Program& program = *pIt;
            LoadProgram( program );
        }
    }

    void ProgramCache::ReloadCache()
    {
        ProgramArray::Iterator it   = m_cache.Begin();
        ProgramArray::Iterator end  = m_cache.End();
        for ( ; it != end; ++it )
        {
            Program& program = *it;

            if ( program.m_handle )
            {
                RenderDevice::DeleteProgram( program.m_handle );
                program.m_handle = 0;
            }

            LoadProgram( program );
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
            Char id[9];
            StringUtils::FormatString( id, 9, "%08x", program.m_id );

            PathString fileName = m_cachePath;
            fileName += id;
            fileName += ".bin";

            SizeT size;
            void * buffer;
            RenderDevice::GetProgramBinary( program.m_handle, buffer, size );

            FileSystem::Save( fileName, buffer, size );
        }
    }

    void ProgramCache::LoadProgramFromBinaries( Program& program )
    {
        Char id[9];
        StringUtils::FormatString( id, 9, "%08x", program.m_id );

        PathString binFileName = m_cachePath;
        binFileName += id;
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

    U32 ProgramCache::CreateId( const Char * name )
    {
        return Core::HashString( name );
    }

    ProgramHandle ProgramCache::FindById( U32 id ) const
    {
        ProgramArray::ConstIterator it  = m_cache.Begin();
        ProgramArray::ConstIterator end = m_cache.End();
        for ( ; it != end; ++it )
        {
            return it - m_cache.Begin();
        }
        return ms_invalidHandle;
    }
}
