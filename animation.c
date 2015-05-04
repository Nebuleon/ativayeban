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
	Animation *a, const char *filename, const int w, const int h,
	const int frameRate)
{
	a->image = IMG_Load(filename);
	if (a->image == NULL) goto bail;
	a->w = w;
	a->h = h;
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
	SDL_FreeSurface(a->image);
}

bool AnimationUpdate(Animation *a, const Uint32 ms)
{
	a->frameCounter += ms;
	const int msPerFrame = 1000 / a->frameRate;
	if (a->frameCounter > msPerFrame)
	{
		a->frameCounter -= msPerFrame;
		a->frame++;
		if (a->frame == (a->image->w / a->w) * (a->image->h / a->h))
		{
			a->frame = 0;
			return true;
		}
	}
	return false;
}
void AnimationDraw(
	const Animation *a, SDL_Surface *screen, const int x, const int y)
{
	const int stride = a->image->w / a->w;
	SDL_Rect src = {
		(Sint16)((a->frame % stride) * a->w),
		(Sint16)((a->frame / stride) * a->h),
		(Sint16)a->w, (Sint16)a->h
	};
	SDL_Rect dest = { (Sint16)x, (Sint16)y, 0, 0 };
	SDL_BlitSurface(a->image, &src, screen, &dest);
}
void AnimationDrawUpperCenter(const Animation *a, SDL_Surface *screen)
{
	AnimationDraw(
		a, screen,
		(SCREEN_WIDTH - a->w) / 2,
		(SCREEN_HEIGHT - a->h) / 2 - SCREEN_HEIGHT / 4);
}
