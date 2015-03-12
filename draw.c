#include <math.h>
#include <stdio.h>

#include "SDL.h"

/*
 * SDL_Surface 32-bit circle-fill algorithm without using trig
 *
 * While I humbly call this "Celdecea's Method", odds are that the
 * procedure has already been documented somewhere long ago.  All of
 * the circle-fill examples I came across utilized trig functions or
 * scanning neighbor pixels.  This algorithm identifies the width of
 * a semi-circle at each pixel height and draws a scan-line covering
 * that width.
 *
 * The code is not optimized but very fast, owing to the fact that it
 * alters pixels in the provided surface directly rather than through
 * function calls.
 *
 * http://content.gpwiki.org/index.php/SDL:Tutorials:Drawing_and_Filling_Circles
 */
void DRAW_FillCircle(SDL_Surface *surface, int cx, int cy, int radius, Uint32 pixel)
{
	if (SDL_MUSTLOCK(surface))
		SDL_LockSurface(surface);
	// Note that there is more to altering the bitrate of this
	// method than just changing this value.  See how pixels are
	// altered at the following web page for tips:
	//   http://www.libsdl.org/intro.en/usingvideo.html
	static const int BPP = 4;

	float r = (float) radius;
	float dy;

	for (dy = 1.0f; dy <= r; dy += 1.0f)
	{
		// This loop is unrolled a bit, only iterating through half of the
		// height of the circle.  The result is used to draw a scan line and
		// its mirror image below it.

		// The following formula has been simplified from our original.  We
		// are using half of the width of the circle because we are provided
		// with a center and we need left/right coordinates.

		float dx = floorf(sqrtf((2.0f * r * dy) - (dy * dy)));
		int x = (int)(cx - dx);
		if (x < 0)
			x = 0;

		// Grab a pointer to the left-most pixel for each half of the circle
		Uint8 *target_pixel_a = (Uint8 *)surface->pixels + ((int)(cy + r - dy)) * surface->pitch + x * BPP;
		Uint8 *target_pixel_b = (Uint8 *)surface->pixels + ((int)(cy - r + dy)) * surface->pitch + x * BPP;

		for (; x <= cx + dx && x < surface->w; x++)
		{
			// Range check in Y (clip to the surface's bounds)
			// for both pixels to be drawn.
			if ((int) (cy + r - dy) >= 0
			 && (int) (cy + r - dy) <  surface->h)
				*(Uint32 *)target_pixel_a = pixel;
			if ((int) (cy - r + dy) >= 0
			 && (int) (cy - r + dy) <  surface->h)
				*(Uint32 *)target_pixel_b = pixel;
			target_pixel_a += BPP;
			target_pixel_b += BPP;
		}
	}
	if (SDL_MUSTLOCK(surface))
		SDL_UnlockSurface(surface);
}