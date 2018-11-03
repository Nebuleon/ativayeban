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
#include "camera.h"

#include "game.h"
#include "utils.h"

// The rate at which the camera tracks the target position
// Every frame it will pan from its current position to the target with
// this ratio
#define CAMERA_TRACK_RATIO 0.1f

Camera camera;

void CameraInit(Camera *c)
{
	// Initialise camera so that players are at the bottom
	c->Y = FIELD_HEIGHT * (0.5f + 0.75f) - PLAYER_RADIUS;
	c->DY = c->Y;
	c->ScrollRate = FIELD_SCROLL;
	c->ScrollCounter = 0;
}

void CameraUpdate(Camera *c, const float playerY, const uint32_t ms)
{
	c->DY -= ms * c->ScrollRate / 1000;
	float targetY;
	if (c->DY < playerY)
	{
		targetY = c->DY;
	}
	else
	{
		targetY = 0.8f * playerY + 0.2f * c->DY;
	}

	c->Y = c->Y * (1 - CAMERA_TRACK_RATIO) + targetY * CAMERA_TRACK_RATIO;

	c->DY = MIN(c->DY, playerY + FIELD_HEIGHT / 2);
	c->ScrollCounter += ms;
	if (c->ScrollCounter >= 1000)
	{
		c->ScrollRate += FIELD_SCROLL_SPEED;
		c->ScrollRate = MIN(FIELD_SCROLL_MAX, c->ScrollRate);
		c->ScrollCounter -= 1000;
	}
	//printf("%f\n", c->ScrollRate);
	// Disable scrolling
	//printf("%f %f\n", c->DY, c->Y);
	//c->Y = playerY;
}
