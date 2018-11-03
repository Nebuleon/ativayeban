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
#include "pickup.h"

#include <stdbool.h>
#include <string.h>

#include "game.h"


typedef struct
{
	float x;
	float y;
} Pickup;

#define PICKUP_RADIUS 0.15f

CArray Pickups;

Tex PickupTex = { NULL, 0, 0 };


void PickupsInit(void)
{
	CArrayInit(&Pickups, sizeof(Pickup));
}
void PickupsFree(void)
{
	CArrayTerminate(&Pickups);
}
void PickupsReset(void)
{
	CArrayClear(&Pickups);
}

void PickupsAdd(const float x, const float y)
{
	Pickup p;
	memset(&p, 0, sizeof p);
	p.x = x;
	p.y = y + PICKUP_RADIUS;
	CArrayPushBack(&Pickups, &p);
}

static bool PickupCollide(
	Pickup *p, const float x, const float y, const float r);
bool PickupsCollide(const float x, const float y, const float r)
{
	for (int i = 0; i < (int)Pickups.size; i++)
	{
		Pickup *p = CArrayGet(&Pickups, i);

		// Remove pickups that are off the top of the screen
		if (p->y > y + FIELD_HEIGHT * 2)
		{
			CArrayDelete(&Pickups, i);
			continue;
		}

		if (PickupCollide(p, x, y, r))
		{
			CArrayDelete(&Pickups, i);
			return true;
		}
	}
	return false;
}
static bool PickupCollide(
	Pickup *p, const float x, const float y, const float r)
{
	const float dx = p->x - x;
	const float dy = p->y - y;
	const float d2 = dx * dx + dy * dy;
	const float rTotal = r + PICKUP_RADIUS;
	const float r2 = rTotal * rTotal;
	return d2 <= r2;
}
static void PickupDraw(const Pickup *p, const float y);
void PickupsDraw(const float y)
{
	for (int i = 0; i < (int)Pickups.size; i++)
	{
		PickupDraw(CArrayGet(&Pickups, i), y);
	}
}
static void PickupDraw(const Pickup *p, const float y)
{
	SDL_Rect dest = {
		(int)SCREEN_X(p->x) - PickupTex.W / 2,
		(int)(SCREEN_Y(p->y) - PickupTex.H / 2 - y),
		PickupTex.W, PickupTex.H
	};
	RenderTex(PickupTex.T, NULL, &dest);
}
