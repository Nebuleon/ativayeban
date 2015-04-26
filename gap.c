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

void GapInit(struct Gap* gap, float y, float gapLeft)
{
	BlockInit(&gap->Left, 0, y, gapLeft);
	BlockInit(
		&gap->Right,
		gapLeft + GAP_WIDTH, y, FIELD_WIDTH - (gapLeft + GAP_WIDTH));
	gap->Passed = false;
	gap->Y = y;
	gap->GapLeft = gapLeft;
	gap->GapRight = gapLeft + GAP_WIDTH;
}
void GapRemove(struct Gap* gap)
{
	BlockRemove(&gap->Left);
	BlockRemove(&gap->Right);
}

void GapDraw(const struct Gap* gap, const float y)
{
	// Draw the left and right parts of the gap
	BlockDraw(&gap->Left, y);
	BlockDraw(&gap->Right, y);
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
