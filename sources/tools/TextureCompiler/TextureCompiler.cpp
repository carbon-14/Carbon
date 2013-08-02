#include "TextureCompiler/TextureCompiler.h"

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cmath>

#if defined( CARBON_PLATFORM_WIN32 )
#include <Windows.h>

HWND hwnd;
HDC hdc;

HGLRC glContext;

PFNGLGENERATEMIPMAPPROC              glGenerateMipmap           = 0;

PFNGLCOMPRESSEDTEXIMAGE3DPROC        glCompressedTexImage3D     = 0;
PFNGLCOMPRESSEDTEXIMAGE2DPROC        glCompressedTexImage2D     = 0;
PFNGLCOMPRESSEDTEXIMAGE1DPROC        glCompressedTexImage1D     = 0;
PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC     glCompressedTexSubImage3D  = 0;
PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC     glCompressedTexSubImage2D  = 0;
PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC     glCompressedTexSubImage1D  = 0;
PFNGLGETCOMPRESSEDTEXIMAGEPROC       glGetCompressedTexImage    = 0;

PFNGLTEXSTORAGE1DPROC                glTexStorage1D             = 0;
PFNGLTEXSTORAGE2DPROC                glTexStorage2D             = 0;
PFNGLTEXSTORAGE3DPROC                glTexStorage3D             = 0;

LRESULT CALLBACK WndProc(HWND p_hWnd, UINT p_uiMessage, WPARAM p_wParam, LPARAM p_lParam)
{
    return DefWindowProc(p_hWnd, p_uiMessage, p_wParam, p_lParam);
}
#endif

#include "libpng/png.h"

unsigned int width, height;
int bit_depth, channels;
unsigned char * data;

bool RecursiveCreateDirectory( const char * dir )
{
#if defined( CARBON_PLATFORM_WIN32 )
    if ( ! CreateDirectory( dir, NULL ) )
    {
        DWORD err = GetLastError();
        if ( err == ERROR_ALREADY_EXISTS )
        {
            return true;
        }
        if ( err == ERROR_PATH_NOT_FOUND )
        {
            char * fileStart;
            char parentDir[ 256 ];
            if ( GetFullPathName( dir, 256, parentDir, &fileStart ) == 0 )
            {
                return false;
            }
            *(fileStart - 1 ) = 0;

            if ( RecursiveCreateDirectory( parentDir ) )
            {
                return ( CreateDirectory( dir, NULL ) != 0 );
            }

            return false;
        }

        return false;
    }
#endif
    return true;
}

bool BuildDirectory( const char * path )
{
#if defined( CARBON_PLATFORM_WIN32 )
    char dir[ 256 ];
    char * fileStart;
    if ( GetFullPathName( path, 256, dir, &fileStart ) == 0 )
    {
        return false;
    }
    *( fileStart - 1 ) = 0;

    if ( ! RecursiveCreateDirectory( dir ) )
    {
        return false;
    }
#endif
    return true;
}

bool InitializeOpenGL()
{
#if defined( CARBON_PLATFORM_WIN32 )
    WNDCLASS wc;
    HINSTANCE hInstance = GetModuleHandle(NULL);

    wc.style = 0;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(1 + COLOR_BTNFACE);
    wc.lpszMenuName =  NULL;
    wc.lpszClassName = "CarbonWndClass";

    if ( !RegisterClass(&wc) ) return false;

    hwnd = CreateWindow( "CarbonWndClass"
                        ,"TextureCompiler"
                        ,WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU
                        ,CW_USEDEFAULT
                        ,CW_USEDEFAULT
                        ,1280
                        ,720
                        ,NULL
                        ,NULL
                        ,hInstance
                        ,NULL );

    if ( !hwnd ) return false;

    hdc = GetDC( hwnd );
    if ( !hdc ) return false;

    PIXELFORMATDESCRIPTOR pfd =
    {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, //Flags
        PFD_TYPE_RGBA,                                              //The kind of framebuffer. RGBA or palette.
        24,                                                         //Colordepth of the framebuffer.
        0, 0, 0, 0, 0, 0,
        0,
        0,
        0,
        0, 0, 0, 0,
        24,                                                         //Number of bits for the depthbuffer
        8,                                                          //Number of bits for the stencilbuffer
        0,                                                          //Number of Aux buffers in the framebuffer.
        PFD_MAIN_PLANE,
        0,
        0, 0, 0
    };

    int pixelFormat = ChoosePixelFormat( hdc, &pfd );
    if ( !pixelFormat ) return false;

    if ( !SetPixelFormat( hdc, pixelFormat, &pfd ) ) return false;

    int attribs[] =
    {
        WGL_CONTEXT_MAJOR_VERSION_ARB   , 4,
        WGL_CONTEXT_MINOR_VERSION_ARB   , 2,
        WGL_CONTEXT_PROFILE_MASK_ARB    , WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0 //End
    };

    HGLRC dummyContext = wglCreateContext( hdc );
    if ( !dummyContext ) return false;

    if ( !wglMakeCurrent( hdc, dummyContext ) )
    {
        wglDeleteContext( dummyContext );
        return false;
    }

    PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress( "wglCreateContextAttribsARB" );

    if ( !wglCreateContextAttribsARB )
    {
        wglDeleteContext( dummyContext );
        return false;
    }

    glContext = wglCreateContextAttribsARB( hdc, 0, attribs );

    wglMakeCurrent( 0, 0 );
    wglDeleteContext( dummyContext );

    if ( !glContext ) return false;

    wglMakeCurrent( hdc, glContext );

    // Load functions
    glGenerateMipmap            = (PFNGLGENERATEMIPMAPPROC)wglGetProcAddress("glGenerateMipmap");

    glCompressedTexImage3D      = (PFNGLCOMPRESSEDTEXIMAGE3DPROC)wglGetProcAddress("glCompressedTexImage3D");
    glCompressedTexImage2D      = (PFNGLCOMPRESSEDTEXIMAGE2DPROC)wglGetProcAddress("glCompressedTexImage2D");
    glCompressedTexImage1D      = (PFNGLCOMPRESSEDTEXIMAGE1DPROC)wglGetProcAddress("glCompressedTexImage1D");
    glCompressedTexSubImage3D   = (PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC)wglGetProcAddress("glCompressedTexSubImage3D");
    glCompressedTexSubImage2D   = (PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC)wglGetProcAddress("glCompressedTexSubImage2D");
    glCompressedTexSubImage1D   = (PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC)wglGetProcAddress("glCompressedTexSubImage1D");
    glGetCompressedTexImage     = (PFNGLGETCOMPRESSEDTEXIMAGEPROC)wglGetProcAddress("glGetCompressedTexImage");

    glTexStorage1D              = (PFNGLTEXSTORAGE1DPROC)wglGetProcAddress("glTexStorage1D");
    glTexStorage2D              = (PFNGLTEXSTORAGE2DPROC)wglGetProcAddress("glTexStorage2D");
    glTexStorage3D              = (PFNGLTEXSTORAGE3DPROC)wglGetProcAddress("glTexStorage3D");

#endif
    return true;
}

void DestroyOpenGL()
{
#if defined( CARBON_PLATFORM_WIN32 )
    wglMakeCurrent( 0, 0 );
    wglDeleteContext( glContext );

    DestroyWindow(hwnd);
    UnregisterClass("CarbonWndClass", GetModuleHandle(NULL));
#endif
}

bool LoadPNG( const char * file_name )
{
    png_structp png_ptr;
    png_infop info_ptr;
    unsigned int sig_read = 0;
    int color_type, interlace_type;
    FILE *fp;

    if (fopen_s(&fp,file_name, "rb"))
        return false;

    /* Create and initialize the png_struct with the desired error handler
    * functions.  If you want to use the default stderr and longjump method,
    * you can supply NULL for the last three parameters.  We also supply the
    * the compiler header file version, so that we know if the application
    * was compiled with a compatible version of the library.  REQUIRED
    */
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL );

    if (png_ptr == NULL)
    {
        fclose(fp);
        return false;
    }

    /* Allocate/initialize the memory for image information.  REQUIRED. */
    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL)
    {
        fclose(fp);
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        return false;
    }

    /* Set error handling if you are using the setjmp/longjmp method (this is
    * the normal method of doing things with libpng).  REQUIRED unless you
    * set up your own error handlers in the png_create_read_struct() earlier.
    */

    if (setjmp(png_jmpbuf(png_ptr)))
    {
        /* Free all of the memory associated with the png_ptr and info_ptr */
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        fclose(fp);
        /* If we get here, we had a problem reading the file */
        return false;
    }

    png_init_io(png_ptr, fp);

    /* If we have already read some of the signature */
    png_set_sig_bytes(png_ptr, sig_read);

    /* The call to png_read_info() gives us all of the information from the
    * PNG file before the first IDAT (image data chunk).  REQUIRED
    */
    png_read_info(png_ptr, info_ptr);

    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
        &interlace_type, NULL, NULL);

    /* Set up the data transformations you want.  Note that these are all
    * optional.  Only call them if you want/need them.  Many of the
    * transformations only work on specific types of images, and many
    * are mutually exclusive.
    */

    /* Tell libpng to strip 16 bit/color files down to 8 bits/color.
    * Use accurate scaling if it's available, otherwise just chop off the
    * low byte.
    */
#ifdef PNG_READ_SCALE_16_TO_8_SUPPORTED
    png_set_scale_16(png_ptr);
#else
    png_set_strip_16(png_ptr);
#endif

    /* Strip alpha bytes from the input data without combining with the
    * background (not recommended).
    */
    //png_set_strip_alpha(png_ptr);

    /* Extract multiple pixels with bit depths of 1, 2, and 4 from a single
    * byte into separate bytes (useful for paletted and grayscale images).
    */
    //png_set_packing(png_ptr);

    /* Change the order of packed pixels to least significant bit first
    * (not useful if you are using png_set_packing). */
    //png_set_packswap(png_ptr);

    /* Expand paletted colors into true RGB triplets */
    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png_ptr);

    /* Expand grayscale images to the full 8 bits from 1, 2, or 4 bits/pixel */
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        png_set_expand_gray_1_2_4_to_8(png_ptr);

    /* Expand paletted or RGB images with transparency to full alpha channels
    * so the data will be available as RGBA quartets.
    */
    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png_ptr);

    /* Set the background color to draw transparent and alpha images over.
    * It is possible to set the red, green, and blue components directly
    * for paletted images instead of supplying a palette index.  Note that
    * even if the PNG file supplies a background, you are not required to
    * use it - you should use the (solid) application background if it has one.
    */

    /*png_color_16 my_background, *image_background;

    if (png_get_bKGD(png_ptr, info_ptr, &image_background))
        png_set_background(png_ptr, image_background,
            PNG_BACKGROUND_GAMMA_FILE, 1, 1.0);
    else
        png_set_background(png_ptr, &my_background,
            PNG_BACKGROUND_GAMMA_SCREEN, 0, 1.0);*/

    /* Some suggestions as to how to get a screen gamma value
    *
    * Note that screen gamma is the display_exponent, which includes
    * the CRT_exponent and any correction for viewing conditions
    */
    double screen_gamma = 2.2;

    /* Tell libpng to handle the gamma conversion for you.  The final call
    * is a good guess for PC generated images, but it should be configurable
    * by the user at run time by the user.  It is strongly suggested that
    * your application support gamma correction.
    */

    int intent;

    if (png_get_sRGB(png_ptr, info_ptr, &intent))
        png_set_gamma(png_ptr, screen_gamma, 0.45455);
    else
    {
        double image_gamma;
        if (png_get_gAMA(png_ptr, info_ptr, &image_gamma))
            png_set_gamma(png_ptr, screen_gamma, image_gamma);
        else
            png_set_gamma(png_ptr, screen_gamma, 0.45455);
    }

#ifdef PNG_READ_QUANTIZE_SUPPORTED
    /* Quantize RGB files down to 8 bit palette or reduce palettes
    * to the number of colors available on your screen.
    */
    if (color_type & PNG_COLOR_MASK_COLOR)
    {
        int num_palette,max_screen_colors;
        png_colorp palette;

        /* This reduces the image to the application supplied palette */
        if (false/* We have our own palette */)
        {
        #define MAX_SCREEN_COLORS 16
            /* An array of colors to which the image should be quantized */
            png_color std_color_cube[MAX_SCREEN_COLORS];

            png_set_quantize(png_ptr, std_color_cube, MAX_SCREEN_COLORS,
                MAX_SCREEN_COLORS, NULL, 0);
        }
        /* This reduces the image to the palette supplied in the file */
        else if (png_get_PLTE(png_ptr, info_ptr, &palette, &num_palette))
        {
            png_uint_16p histogram = NULL;

            png_get_hIST(png_ptr, info_ptr, &histogram);

            png_set_quantize(png_ptr, palette, num_palette,
                max_screen_colors, histogram, 0);
        }
    }
#endif /* PNG_READ_QUANTIZE_SUPPORTED */

    /* Invert monochrome files to have 0 as white and 1 as black */
    /*png_set_invert_mono(png_ptr);*/

    /* If you want to shift the pixel values from the range [0,255] or
    * [0,65535] to the original [0,7] or [0,31], or whatever range the
    * colors were originally in:
    */
    /*if (png_get_valid(png_ptr, info_ptr, PNG_INFO_sBIT))
    {
        png_color_8p sig_bit_p;

        png_get_sBIT(png_ptr, info_ptr, &sig_bit_p);
        png_set_shift(png_ptr, sig_bit_p);
    }*/

    /* Flip the RGB pixels to BGR (or RGBA to BGRA) */
    /*if (color_type & PNG_COLOR_MASK_COLOR)
        png_set_bgr(png_ptr);*/

    /* Swap the RGBA or GA data to ARGB or AG (or BGRA to ABGR) */
    /*png_set_swap_alpha(png_ptr);*/

    /* Swap bytes of 16 bit files to least significant byte first */
    /*png_set_swap(png_ptr);*/

    /* Add filler (or alpha) byte (before/after each RGB triplet) */
    /*png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);*/

    /* Optional call to gamma correct and add the background to the palette
    * and update info structure.  REQUIRED if you are expecting libpng to
    * update the palette for you (ie you selected such a transform above).
    */
    png_read_update_info(png_ptr, info_ptr);

    /* Allocate the memory to hold the image using the fields of info_ptr. */

    /* The easiest way to read the image: */
    png_uint_32 rowbytes = png_get_rowbytes(png_ptr, info_ptr);
    channels = (int)png_get_channels(png_ptr, info_ptr);
  
    if ((data = (unsigned char *)malloc(rowbytes*height)) == NULL)
    {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        return false;
    }

    memset(data,0,rowbytes*height);

    png_bytepp row_pointers = (png_bytepp)png_malloc(png_ptr, height * sizeof(png_bytep) );
    for (png_uint_32 i = 0;  i < height;  ++i)
        row_pointers[i] = data + i*rowbytes;

    /* Now it's time to read the image.  One of these methods is REQUIRED */
    /* Read the entire image in one go */
    png_read_image(png_ptr, row_pointers);

    /* Read rest of file, and get additional chunks in info_ptr - REQUIRED */
    png_read_end(png_ptr, info_ptr);

    /* At this point you have read the entire image */

    /* Clean up after the read, and free any memory allocated - REQUIRED */
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

    /* Close the file */
    fclose(fp);

    /* That's it */
    return true;
}

const int colorMask[ 5 ] = { -1, 0, -1, 1, 2 }; // greyscale, rgb, rgba selector

const GLint internalFormatTable[ 2 * TP_COUNT ] [ 3 ] =
{
    { GL_R8             , GL_SRGB8              , GL_SRGB8_ALPHA8               },   // TP_COLOR
    { GL_R8             , GL_RGB8               , GL_RGBA8                      },   // TP_LINEAR
    { 0                 , GL_RG8                , GL_RG8                        },   // TP_NORMAL
    { GL_COMPRESSED_RED , GL_COMPRESSED_SRGB    , GL_COMPRESSED_SRGB_ALPHA      },   // TP_COLOR    - COMPRESSED -
    { GL_COMPRESSED_RED , GL_COMPRESSED_RGB     , GL_COMPRESSED_RGBA            },   // TP_LINEAR   - COMPRESSED -
    { 0                 , GL_COMPRESSED_RG      , GL_COMPRESSED_RG              },   // TP_NORMAL   - COMPRESSED -
};

const GLint externalFormatTable[ TP_COUNT ] [ 3 ] =
{
    { 0         , GL_BGR    , GL_BGRA   },   // TP_COLOR
    { GL_RED    , GL_BGR    , GL_BGRA   },   // TP_LINEAR
    { 0         , GL_RG     , GL_RG     }    // TP_NORMAL
};

struct TextureHeader
{
    unsigned int    internalFormat;
    unsigned int    externalFormat;
    unsigned int    mipMapCount;
    bool            compressed;
};

struct LevelDesc
{
    unsigned int    size;
    unsigned int    width;
    unsigned int    height;
};

bool BuildTexture( const char * outFilename, TextureProfile profile, int options )
{
    GLuint map;
    glGenTextures(1, &map);
    glBindTexture(GL_TEXTURE_2D, map);
    
    if ( channels < 0 || channels > 4 )
        return false;

    int mask = colorMask[ channels ];

    if ( mask < 0 )
        return false;

    bool compress = (options & OT_COMPRESS) != 0;
    bool mipmap = (options & OT_MIPMAPGEN) != 0;

    GLint internal_format = internalFormatTable[ profile + (compress ? TP_COUNT : 0) ] [ mask ];
    GLint external_format = externalFormatTable[ profile ] [ mask ];

    GLint testFormat = GL_COMPRESSED_RG;

    if ( internal_format == 0 )
        return false;
    
    if ( profile == TP_NORMAL )
    {
        // rearrange data
        const size_t offset = channels;
        unsigned char * in = data;
        unsigned char * out = data;
        const unsigned char * end = in + channels*width*height;
        while ( in != end )
        {
            // normalize
            double x = (double)in[0] * 2.0 / 255.0 - 1.0;
            double y = (double)in[1] * 2.0 / 255.0 - 1.0;
            double z = (double)in[2] * 2.0 / 255.0 - 1.0;

            double n = sqrt( x * x + y * y + z * z );

            x = 255.0 * ( 0.5 * x / n + 0.5 );
            y = 255.0 * ( 0.5 * y / n + 0.5 );

            out[0] = (unsigned char)x;
            out[1] = (unsigned char)y;

            in += channels;
            out += 2;
        }
    }
    else if ( channels != 1 )
    {
        // swap red & blue channels
        const size_t offset = channels;
        unsigned char * pix = data;
        const unsigned char * end = pix + channels*width*height;
        while ( pix != end )
        {
            pix[0] = pix[2] - pix[0];
            pix[2] = pix[2] - pix[0];
            pix[0] = pix[2] + pix[0];
            pix += channels;
        }
    }

    unsigned int levelCount = 1;
    if ( mipmap )
    {
        levelCount = ( width < height ) ? height : width;
        levelCount = (unsigned int)( std::log( (double)levelCount ) / std::log( 2.0 ) ) + 1;
    }

    if ( compress )
    {
        glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, external_format, GL_UNSIGNED_BYTE, data);        

        GLint compressed;
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED, &compressed);
        if ( compressed == GL_FALSE )
        {
            free( data );
            glDeleteTextures( 1, &map );
            return false;
        }
    }
    else
    {
        glTexStorage2D( GL_TEXTURE_2D, levelCount, internal_format, width, height );
        glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, width, height, external_format, GL_UNSIGNED_BYTE, data );
    }

    if ( mipmap )
    {
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    }

    if ( ! BuildDirectory( outFilename ) )
    {
        glDeleteTextures( 1, &map );
        return false;
    }

    FILE *fp;

    if (fopen_s(&fp, outFilename, "wb"))
    {
        glDeleteTextures( 1, &map );
        return false;
    }

    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &internal_format);

    TextureHeader header;
    header.internalFormat   = internal_format;
    header.externalFormat   = external_format;
    header.mipMapCount      = levelCount;
    header.compressed       = compress;

    fwrite(&header,1,sizeof(header),fp);

    // level 0
    GLint img_width     = width;
    GLint img_height    = height;

    GLint img_size;
    if ( compress )
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &img_size);
    else
        img_size = img_width * img_height * channels * ( bit_depth / 8 );

    LevelDesc levelDesc;
    levelDesc.size      = img_size;
    levelDesc.width     = img_width;
    levelDesc.height    = img_height;

    unsigned char * img = (unsigned char *)malloc(img_size * sizeof(unsigned char));

    if ( compress )
        glGetCompressedTexImage(GL_TEXTURE_2D, 0, img );
    else
        glGetTexImage(GL_TEXTURE_2D, 0, external_format, GL_UNSIGNED_BYTE, img );

    fwrite(&levelDesc,1,sizeof(levelDesc),fp);
    fwrite(img,1,img_size,fp);

    // mip maps
    for ( unsigned int i=1; i<levelCount; ++i )
    {
        glGetTexLevelParameteriv(GL_TEXTURE_2D, i, GL_TEXTURE_WIDTH                 , &img_width);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, i, GL_TEXTURE_HEIGHT                , &img_height);

        if ( compress )
            glGetTexLevelParameteriv(GL_TEXTURE_2D, i, GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &img_size);
        else
            img_size = img_width * img_height * channels * ( bit_depth / 8 );

        levelDesc.size      = img_size;
        levelDesc.width     = img_width;
        levelDesc.height    = img_height;

        if ( compress )
            glGetCompressedTexImage(GL_TEXTURE_2D, i, img );
        else
            glGetTexImage(GL_TEXTURE_2D, i, external_format, GL_UNSIGNED_BYTE, img );

        fwrite(&levelDesc,1,sizeof(levelDesc),fp);
        fwrite(img,1,img_size,fp);
    }

    fclose(fp);

    free( img );
    glDeleteTextures( 1, &map );

    return true;
}

bool CompileTexture( const char * filename, const char * dir, TextureProfile profile, int options )
{
    char inFilename[256];
    sprintf( inFilename, "%s/data/%s", dir, filename );

    size_t len = strlen( inFilename );

    if ( len < 5 )
        return false;

    const char * ext = inFilename - 3 + len;
    
    // only png files for now
    if ( strcmp( ext, "png" ) == 0 || strcmp( ext, "PNG" ) == 0 )
    {
        if ( ! LoadPNG( inFilename ) )
            return false;
    }
    else
    {
        return false;
    }

    // only 8-bits per channel for now
    if ( bit_depth != 8 )
    {
        return false;
    }

    if ( ! InitializeOpenGL() )
    {
        free( data );
        return false;
    }

    char outFilename[256];
    sprintf( outFilename, "%s/cache/%s", dir, filename );
    len = strlen( outFilename );
    strcpy( outFilename + len - 3, "btx" );

    bool success = BuildTexture( outFilename, profile, options );

    DestroyOpenGL();

    return success;
}
