#pragma once
#ifndef _GRAPHIC_RASTERIZER_H
#define _GRAPHIC_RASTERIZER_H

#include "Graphic/ProgramCache.h"

#include "Core/Matrix.h"

namespace Graphic
{
    class Camera;

    class _GraphicExport Rasterizer
    {
    public:
        struct Context
        {
            const Camera *  m_camera;
            SizeT           m_width;
            SizeT           m_height;

            Array< Vector > m_vPlanes;
            Array< Vector > m_hPlanes;

            Handle          m_quadBuffer;
            SizeT           m_quadBufferSize;
        };

    public:
        Rasterizer();
        ~Rasterizer();

        void Initialize();
        void Destroy();

        static Context * CreateContext();
        static void UpdateContext( Context * context, SizeT width, SizeT height, const Camera * camera );
        static void DestroyContext( Context * context );

        template < typename I, typename O >
        void Render( const I * const * input, O * output, SizeT count, Context * context ) const;
    };

    template < typename I, typename O >
    void Rasterizer::Render( const I * const * input, O * output, SizeT count, Context * context ) const
    {
        const I * const * end = input + count;
        for ( ; input!=end; ++input, ++output )
        {
            Vector v = (*input)->GetBoundingSphere();
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

            for ( SizeT i=0; i<4; ++i )
            {
                if ( inside[i] )
                {
                    const Vector * planes[2];
                    planes[0] = quad[2*(i/2)];
                    planes[1] = quad[2*(i/2)+1];

                    SizeT dist = planes[1] - planes[0];
                    while ( dist > 1 )
                    {
                        const Vector * m = planes[0] + dist / 2;

                        test = Dot( c, *m ) > r;

                        F128 result;
                        Store( result, test );

                        if ( result[i] )    { planes[0] = m; }
                        else                { planes[1] = m; }

                        dist = planes[1] - planes[0];
                    }

                    quad[i] = planes[i%2];
                }
            }

            output->m_quad[0] = quad[0] - context->m_vPlanes.Begin();
            output->m_quad[1] = quad[1] - context->m_vPlanes.Begin();
            output->m_quad[2] = quad[2] - context->m_hPlanes.Begin();
            output->m_quad[3] = quad[3] - context->m_hPlanes.Begin();
        }
    }
}

#endif // _GRAPHIC_RASTERIZER_H
