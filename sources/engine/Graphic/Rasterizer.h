#pragma once
#ifndef _GRAPHIC_RASTERIZER_H
#define _GRAPHIC_RASTERIZER_H

#include "Graphic/ProgramCache.h"

#include "Core/Vector.h"

namespace Graphic
{
    class Camera;

    class _GraphicExport Rasterizer
    {
    public:
        struct Context
        {
            const Camera *          m_camera;
            Handle                  m_linearDepthTexture;
            SizeT                   m_width;
            SizeT                   m_height;
            SizeT                   m_size;

            Array< Vector >         m_vPlanes;
            Array< Vector >         m_hPlanes;

            Handle                  m_depthBuffer;
            Array< U8 >             m_mapCount;
            Array< const Vector * > m_map;

            Array< Vector >         m_spheres;
        };

    public:
        Rasterizer();
        ~Rasterizer();

        void Initialize();
        void Destroy();

        static Context * CreateContext();
        static void UpdateContext( Context * context, SizeT width, SizeT height, const Camera * camera, Handle linearDepthTexture );
        static void DestroyContext( Context * context );

        template< typename T >
        void Render( const T * const * objs, SizeT objCount, Context * context ) const;
        void Draw( const Context * context ) const;

    private:
        static const SizeT  ms_tileSize = 16;
        static const SizeT  ms_maxCount = 64;

        void FillQuad( const Vector * quad[4], const Vector * sphere, Context * context ) const;

        ProgramHandle       m_programTileDepth;
    };

    template < typename T >
    void Rasterizer::Render( const T * const * objs, SizeT objCount, Context * context ) const
    {
        context->m_spheres.Resize( objCount );

        for ( SizeT i=0; i<objCount; ++i )
        {
            context->m_spheres[ i ] = objs[ i ]->m_sphere;
        }

        Array< Vector >::ConstIterator it = context->m_spheres.Begin();
        Array< Vector >::ConstIterator end = context->m_spheres.End();
        for ( ; it!=end; ++it )
        {
            Vector v = *it;
            Vector c = Select( v, One4, Mask<0,0,0,1>() );
            Vector r = Swizzle<3,3,3,3>( v );

            const Vector * quad[4] =
            {
                context->m_vPlanes.Begin(),
                context->m_vPlanes.End() - 1,
                context->m_hPlanes.Begin(),
                context->m_hPlanes.End() - 1
            };

            Matrix clip = { *quad[0], -*quad[1], *quad[2], -*quad[3] };
            clip = Transpose( clip );

            Vector test = Mul( clip, c ) > r;

            F128 inside;
            Store( inside, test );

            r = r * Vector4( 1.0f, -1.0f, 1.0f, -1.0f );

            if ( inside[0] )
            {
                const Vector * minPlane = quad[0];
                const Vector * maxPlane = quad[1];
                SizeT dist = maxPlane - minPlane;
                while ( dist > 1 )
                {
                    quad[0] = minPlane + dist / 2;

                    test = Dot( c, *quad[0] ) < r;

                    F128 result;
                    Store( result, test );

                    if ( result[0] )    { maxPlane = quad[0]; }
                    else                { minPlane = quad[0]; }

                    dist = maxPlane - minPlane;
                }
            }

            if ( inside[1] )
            {
                const Vector * minPlane = quad[0];
                const Vector * maxPlane = quad[1];
                SizeT dist = maxPlane - minPlane;
                while ( dist > 1 )
                {
                    quad[1] = minPlane + dist / 2;

                    test = Dot( c, *quad[1] ) < r;
                    
                    F128 result;
                    Store( result, test );

                    if ( result[1] )    { maxPlane = quad[1]; }
                    else                { minPlane = quad[1]; }

                    dist = maxPlane - minPlane;
                }
            }

            if ( inside[2] )
            {
                const Vector * minPlane = quad[2];
                const Vector * maxPlane = quad[3];
                SizeT dist = maxPlane - minPlane;
                while ( dist > 1 )
                {
                    quad[2] = minPlane + dist / 2;

                    test = Dot( c, *quad[2] ) < r;
                    
                    F128 result;
                    Store( result, test );

                    if ( result[2] )    { maxPlane = quad[2]; }
                    else                { minPlane = quad[2]; }

                    dist = maxPlane - minPlane;
                }
            }

            if ( inside[3] )
            {
                const Vector * minPlane = quad[2];
                const Vector * maxPlane = quad[3];
                SizeT dist = maxPlane - minPlane;
                while ( dist > 1 )
                {
                    quad[3] = minPlane + dist / 2;

                    test = Dot( c, *quad[3] ) < r;
                    
                    F128 result;
                    Store( result, test );

                    if ( result[3] )    { maxPlane = quad[3]; }
                    else                { minPlane = quad[3]; }

                    dist = maxPlane - minPlane;
                }
            }

            FillQuad( quad, it, context );
        }
    }
}

#endif // _GRAPHIC_RASTERIZER_H
