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
#define GAP_SPRITE_HEIGHT 15

SDL_Surface *GapSurfaces[6];

static cpBody *MakeGapBody(const float left, const float right, const float y);
static SDL_Surface *RandomGapSurface(void);
void GapInit(struct Gap* gap, float y, float gapLeft)
{
	gap->BodyLeft = MakeGapBody(0, gapLeft, y);
	gap->BodyRight = MakeGapBody(gapLeft + GAP_WIDTH, FIELD_WIDTH, y);
	gap->Passed = false;
	gap->Y = y;
	gap->GapLeft = gapLeft;
	gap->GapRight = gapLeft + GAP_WIDTH;
	gap->GapLeftSurface = RandomGapSurface();
	gap->GapRightSurface = RandomGapSurface();
}
void GapRemove(struct Gap* gap)
{
	cpSpaceRemoveBody(Space, gap->BodyLeft);
	cpSpaceRemoveBody(Space, gap->BodyRight);
}
static cpBody *MakeGapBody(const float left, const float right, const float y)
{
	cpBody *body = cpSpaceAddBody(Space, cpBodyNewStatic());
	cpBodySetPosition(body, cpv((left + right) / 2, y - GAP_HEIGHT / 2));
	cpShape *shape = cpSpaceAddShape(
		Space, cpBoxShapeNew(body, right - left, GAP_HEIGHT, 0.0));
	cpShapeSetElasticity(shape, 1.0f);
	cpShapeSetFriction(shape, 1.0f);
	return body;
}
static SDL_Surface *RandomGapSurface(void)
{
	return GapSurfaces[rand() % 6];
}

static void GapDrawPart(const int x, const int y, SDL_Surface *s);
void GapDraw(const struct Gap* gap, const float y)
{
	// Draw the left and right parts of the gap

	// Left part
	const cpVect posL = cpBodyGetPosition(gap->BodyLeft);
	GapDrawPart(
		SCREEN_X((float)posL.x + gap->GapLeft / 2) - GAP_SPRITE_WIDTH,
		SCREEN_Y((float)posL.y + GAP_HEIGHT / 2) - (int)y,
		gap->GapLeftSurface);

	// Right part
	const cpVect posR = cpBodyGetPosition(gap->BodyRight);
	GapDrawPart(
		SCREEN_X((float)posR.x - (FIELD_WIDTH - gap->GapRight) / 2),
		SCREEN_Y((float)posR.y + GAP_HEIGHT / 2) - (int)y,
		gap->GapRightSurface);
}
static void GapDrawPart(const int x, const int y, SDL_Surface *s)
{
	SDL_Rect dest = { (Sint16)x, (Sint16)y, 0, 0 };
	SDL_BlitSurface(s, NULL, Screen, &dest);
}

float GapBottom(const struct Gap* gap)
{
	return gap->Y - GAP_HEIGHT;
}

bool GapSurfacesLoad(void)
{
	for (int i = 0; i < 6; i++)
	{
		char buf[256];
		sprintf(buf, "data/graphics/floor%d.png", i);
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
