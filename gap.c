/*
Copyright (c) 2015, Cong Xu
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.
Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/
#include "gap.h"

#include <math.h>

#include "SDL_image.h"

#include "game.h"
#include "main.h"

#define GAP_SPRITE_WIDTH 318
#define GAP_HEIGHT 15

SDL_Surface *GapSurfaces[6];

static SDL_Surface *RandomGapSurface(void);
void GapInit(struct Gap* gap, float y, float gapLeft)
{
	gap->Passed = false;
	gap->Y = y;
	gap->GapLeft = gapLeft;
	gap->GapRight = gapLeft + GAP_WIDTH;
	gap->GapLeftSurface = RandomGapSurface();
	gap->GapRightSurface = RandomGapSurface();
}
static SDL_Surface *RandomGapSurface(void)
{
	return GapSurfaces[rand() % 6];
}

bool GapUpdate(struct Gap* gap, float scroll)
{
	// Update scroll.
	gap->Y += scroll;

	return GapBottom(gap) > FIELD_HEIGHT;
}

void GapDraw(const struct Gap* gap)
{
	// Draw the left and right parts of the gap, using random sprites

	// Left part
	int x = SCREEN_X(gap->GapLeft) - GAP_SPRITE_WIDTH;
	int y = SCREEN_Y(gap->Y);
	SDL_Rect dest = { (Sint16)x, (Sint16)y, 0, 0 };
	SDL_BlitSurface(gap->GapLeftSurface, NULL, Screen, &dest);

	// Right part
	x = SCREEN_X(gap->GapRight);
	dest.x = (Sint16)x;
	dest.y = (Sint16)y;
	SDL_BlitSurface(gap->GapRightSurface, NULL, Screen, &dest);
}

float GapBottom(const struct Gap* gap)
{
	return gap->Y - GAP_HEIGHT * FIELD_HEIGHT / SCREEN_HEIGHT;
}

bool GapIsOn(const struct Gap* gap, float x, float radius)
{
	// TODO Circle physics.
	return x - radius <= gap->GapLeft || x + radius >= gap->GapRight;
}

bool GapSurfacesLoad(void)
{
	for (int i = 0; i < 6; i++)
	{
		char buf[256];
		sprintf(buf, "floor%d.png", i);
		GapSurfaces[i] = IMG_Load(buf);
		if (GapSurfaces[i] == NULL)
		{
			return false;
		}
	}
	return true;
}
void GapSurfacesFree(void)
{
	for (int i = 0; i < 6; i++)
	{
		SDL_FreeSurface(GapSurfaces[i]);
	}
}
