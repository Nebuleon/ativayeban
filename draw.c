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
#include "draw.h"

#include <SDL_image.h>

#include "main.h"


static Tex Surf2Tex(SDL_Surface *s);

Tex LoadTex(const char *filename)
{
	SDL_Surface* s = IMG_Load(filename);
	if (s == NULL)
	{
		printf("Failed to IMG_Load: %s\n", SDL_GetError());
		SDL_ClearError();
		Tex t = { NULL, 0, 0 };
		return t;
	}
	return Surf2Tex(s);
}

Tex LoadText(const char *text, TTF_Font *font, const SDL_Color c)
{
	SDL_Surface *s = TTF_RenderText_Blended(font, text, c);
	if (s == NULL)
	{
		printf("Failed to render text: %s\n", SDL_GetError());
		SDL_ClearError();
		Tex t = { NULL, 0, 0 };
		return t;
	}
	return Surf2Tex(s);
}

static Tex Surf2Tex(SDL_Surface *s)
{
	Tex t = { NULL, 0, 0 };
	t.T = SDL_CreateTextureFromSurface(Renderer, s);
	SDL_FreeSurface(s);
	if (SDL_QueryTexture(t.T, NULL, NULL, &t.W, &t.H) != 0)
	{
		printf("Failed to query texture: %s\n", SDL_GetError());
		SDL_ClearError();
	}
	return t;
}

void RenderTex(SDL_Texture *t, const SDL_Rect *src, const SDL_Rect *dst)
{
	if (SDL_RenderCopy(Renderer, t, src, dst) != 0)
	{
		printf("Failed to render texture: %s\n", SDL_GetError());
		SDL_ClearError();
	}
}
