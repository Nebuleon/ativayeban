#ifndef _GAP_H_
#define _GAP_H_

#include <stdbool.h>

#include "SDL.h"

// Gaps are a pair of rectangles with a gap in between.
// The player scores after falling through a gap.

struct Gap
{
	// Where the gap layer is.
	float Y;
	// Where the gap starts and ends
	float GapLeft;
	float GapRight;

	bool Passed;
};

struct GapSurfaces
{
	SDL_Surface* Left;
	SDL_Surface* Middle;
	SDL_Surface* Right;
};
extern struct GapSurfaces GapSurfaces;

extern void GapInit(struct Gap* gap, float y, float gapX);
extern bool GapUpdate(struct Gap* gap, float scroll);
extern void GapDraw(const struct Gap* gap);

extern float GapBottom(const struct Gap* gap);
extern bool GapIsOn(const struct Gap* gap, float x, float radius);

extern bool GapSurfacesLoad(struct GapSurfaces* surfaces);
extern void GapSurfacesFree(struct GapSurfaces* surfaces);

#endif /* !defined(_GAP_H_) */
