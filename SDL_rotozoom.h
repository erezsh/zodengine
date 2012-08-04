
/*

 SDL_rotozoom - rotozoomer

 LGPL (c) A. Schiffler

*/

#ifndef _SDL_rotozoom_h
#define _SDL_rotozoom_h

#include <math.h>

/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C" {
#endif

#ifndef M_PI
#define M_PI	3.141592654
#endif

#include "SDL/SDL.h"

/* ---- Defines */

#define SMOOTHING_OFF		0
#define SMOOTHING_ON		1

/* ---- Structures */

    typedef struct tColorRGBA {
	Uint8 r;
	Uint8 g;
	Uint8 b;
	Uint8 a;
    } tColorRGBA;

    typedef struct tColorY {
	Uint8 y;
    } tColorY;


/* ---- Prototypes */

#ifdef WIN32
#  ifdef DLL_EXPORT
#    define SDL_ROTOZOOM_SCOPE __declspec(dllexport)
#  else
#    ifdef LIBSDL_GFX_DLL_IMPORT
#      define SDL_ROTOZOOM_SCOPE __declspec(dllimport)
#    endif
#  endif
#endif
#ifndef SDL_ROTOZOOM_SCOPE
#  define SDL_ROTOZOOM_SCOPE extern
#endif

/* 
 
 rotozoomSurface()

 Rotates and zoomes a 32bit or 8bit 'src' surface to newly created 'dst' surface.
 'angle' is the rotation in degrees. 'zoom' a scaling factor. If 'smooth' is 1
 then the destination 32bit surface is anti-aliased. If the surface is not 8bit
 or 32bit RGBA/ABGR it will be converted into a 32bit RGBA format on the fly.

*/

    SDL_ROTOZOOM_SCOPE SDL_Surface *rotozoomSurface(SDL_Surface * src, double angle, double zoom, int smooth);

    SDL_ROTOZOOM_SCOPE SDL_Surface *rotozoomSurfaceXY
    (SDL_Surface * src, double angle, double zoomx, double zoomy, int smooth);

/* Returns the size of the target surface for a rotozoomSurface() call */

    SDL_ROTOZOOM_SCOPE void rotozoomSurfaceSize(int width, int height, double angle, double zoom, int *dstwidth,
					  int *dstheight);

    SDL_ROTOZOOM_SCOPE void rotozoomSurfaceSizeXY
    (int width, int height, double angle, double zoomx, double zoomy, 
     int *dstwidth, int *dstheight);

/* 
 
 zoomSurface()

 Zoomes a 32bit or 8bit 'src' surface to newly created 'dst' surface.
 'zoomx' and 'zoomy' are scaling factors for width and height. If 'smooth' is 1
 then the destination 32bit surface is anti-aliased. If the surface is not 8bit
 or 32bit RGBA/ABGR it will be converted into a 32bit RGBA format on the fly.

*/

    SDL_ROTOZOOM_SCOPE SDL_Surface *zoomSurface(SDL_Surface * src, double zoomx, double zoomy, int smooth);

/* Returns the size of the target surface for a zoomSurface() call */

    SDL_ROTOZOOM_SCOPE void zoomSurfaceSize(int width, int height, double zoomx, double zoomy, int *dstwidth, int *dstheight);


/* 
    shrinkSurface()

    Shrinks a 32bit or 8bit 'src' surface ti a newly created 'dst' surface.
    'factorx' and 'factory' are the shrinking ratios (i.e. 2=1/2 the size,
    3=1/3 the size, etc.) The destination surface is antialiased by averaging
    the source box RGBA or Y information. If the surface is not 8bit
    or 32bit RGBA/ABGR it will be converted into a 32bit RGBA format on the fly.
*/     
    
    SDL_ROTOZOOM_SCOPE SDL_Surface *shrinkSurface(SDL_Surface * src, int factorx, int factory);

/* 

    Other functions
    
*/

    SDL_ROTOZOOM_SCOPE SDL_Surface* rotateSurface90Degrees(SDL_Surface* pSurf, int numClockwiseTurns);

/* Ends C function definitions when using C++ */
#ifdef __cplusplus
}
#endif

#endif				/* _SDL_rotozoom_h */
