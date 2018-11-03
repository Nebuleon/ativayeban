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
#include "box.h"

#include <SDL_image.h>

#include "draw.h"
#include "game.h"
#include "gap.h"
#include "main.h"
#include "space.h"
#include "sys_config.h"
#include "utils.h"

#define BLOCK_SPRITE_HEIGHT 15

Tex BoxTexes[6];

static cpBody *MakeBody(const float x, const float y, const float w);
static Tex RandomTex(void);
void BlockInit(Block *block, const float x, const float y, const float w)
{
	block->Body = MakeBody(x, y, w);
	block->W = w;
	block->H = GAP_HEIGHT;
	block->T = RandomTex();
}
static cpBody *MakeBody(const float x, const float y, const float w)
{
	cpBody *body = cpSpaceAddBody(space.Space, cpBodyNewStatic());
	cpBodySetPosition(body, cpv(x + w / 2, y - GAP_HEIGHT / 2));
	cpShape *shape = cpSpaceAddShape(
		space.Space, cpBoxShapeNew(body, w, GAP_HEIGHT, 0.0));
	cpShapeSetElasticity(shape, BLOCK_ELASTICITY);
	cpShapeSetFriction(shape, 1.0f);
	return body;
}
static Tex RandomTex(void)
{
	return BoxTexes[rand() % 6];
}

static void RemoveShape(cpBody *body, cpShape *shape, void *data);
void BlockRemove(Block *block)
{
	cpBodyEachShape(block->Body, RemoveShape, space.Space);
	cpSpaceRemoveBody(space.Space, block->Body);
}
static void RemoveShape(cpBody *body, cpShape *shape, void *data)
{
	UNUSED(body);
	cpSpace *s = data;
	cpSpaceRemoveShape(s, shape);
	cpShapeFree(shape);
}

void BlockDraw(const Block *block, const float y)
{
	const cpVect pos = cpBodyGetPosition(block->Body);
	SDL_Rect src = { 0, 0,  SCREEN_X(block->W), block->T.H };
	SDL_Rect dest =
	{
		(int)SCREEN_X((float)pos.x - block->W / 2),
		(int)(SCREEN_Y((float)pos.y + block->H / 2) - y),
		src.w, src.h
	};
	RenderTex(block->T.T, &src, &dest);
}


bool BoxTexesLoad(void)
{
	for (int i = 0; i < 6; i++)
	{
		char buf[256];
		sprintf(buf, DATA_DIR "graphics/floor%d.png", i);
		BoxTexes[i] = LoadTex(buf);
		if (BoxTexes[i].T == NULL)
		{
			return false;
		}
	}
	return true;
}
void BoxTexesFree(void)
{
	for (int i = 0; i < 6; i++)
	{
		SDL_DestroyTexture(BoxTexes[i].T);
	}
}
