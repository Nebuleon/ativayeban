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
#include "animation.h"

#include <SDL_image.h>

#include "init.h"


bool AnimationLoad(
	Animation *a, const char *fileFormat, const int frames,
	const int frameRate)
{
	CArrayInit(&a->images, sizeof(SDL_Surface *));
	for (int i = 0; i < frames; i++)
	{
		char buf[256];
		sprintf(buf, fileFormat, i + 1);
		SDL_Surface *s = IMG_Load(buf);
		if (s == NULL)
		{
			goto bail;
		}
		CArrayPushBack(&a->images, &s);
	}
	a->frame = 0;
	a->frameCounter = 0;
	a->frameRate = frameRate;

	return true;

bail:
	AnimationFree(a);
	return false;
}
void AnimationFree(Animation *a)
{
	for (int i = 0; i < (int)a->images.size; i++)
	{
		SDL_Surface **s = CArrayGet(&a->images, i);
		SDL_FreeSurface(*s);
	}
	CArrayTerminate(&a->images);
}

void AnimationUpdate(Animation *a, const Uint32 ms)
{
	a->frameCounter += ms;
	const int msPerFrame = 1000 / a->frameRate;
	if (a->frameCounter > msPerFrame)
	{
		a->frameCounter -= msPerFrame;
		a->frame++;
		if (a->frame == (int)a->images.size)
		{
			a->frame = 0;
		}
	}
}
void AnimationDrawUpperCenter(const Animation *a, SDL_Surface *screen)
{
	SDL_Surface **frame = CArrayGet(&a->images, a->frame);
	SDL_Rect dest = {
		(Sint16)((SCREEN_WIDTH - (*frame)->w) / 2),
		(Sint16)((SCREEN_HEIGHT - (*frame)->h) / 2 - SCREEN_HEIGHT / 4),
		0,
		0
	};
	SDL_BlitSurface(*frame, NULL, screen, &dest);
}