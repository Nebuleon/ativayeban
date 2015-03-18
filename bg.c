/*
 * Ativayeban, background rendering code file
 * Copyright (C) 2014 Nebuleon Fumika <nebuleon@gcw-zero.com>
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#include "bg.h"

#include <SDL_image.h>

#include "main.h"

#define SCROLL_FACTOR 0.01f
#define SCALE_1 1.0f
#define SCALE_2 2.0f
#define SCALE_3 3.0f

Backgrounds BG;

void BackgroundsInit(Backgrounds *bg)
{
	bg->scroll = 0;
}

void AdvanceBackground(Backgrounds *bg, uint32_t Milliseconds)
{
	bg->scroll += Milliseconds * SCROLL_FACTOR;
}

static void DrawBackgroundScroll(
	SDL_Surface *bg, const float scroll, const float factor);
void DrawBackground(Backgrounds *bg)
{
	SDL_BlitSurface(bg->Layer0, NULL, Screen, NULL);
	DrawBackgroundScroll(bg->Layer1, bg->scroll, SCALE_1);
	DrawBackgroundScroll(bg->Layer2, bg->scroll, SCALE_2);
	DrawBackgroundScroll(bg->Layer3, bg->scroll, SCALE_3);
}
static void DrawBackgroundScroll(
	SDL_Surface *bg, const float scroll, const float factor)
{
	SDL_Rect src = { 0, 0, 0, 0 };
	src.w = (Uint16)Screen->w;
	src.h = (Uint16)Screen->h;
	const int s = (int)(scroll * factor) % bg->h;
	src.y = (Sint16)s;
	SDL_BlitSurface(bg, &src, Screen, NULL);
	if (bg->h - s < Screen->h)
	{
		SDL_Rect dst = { 0, 0, 0, 0 };
		dst.y = (Sint16)(bg->h - s);
		SDL_BlitSurface(bg, NULL, Screen, &dst);
	}
}

bool BackgroundsLoad(Backgrounds* bg)
{
#define LOAD_SURFACE(_surface, _filename)\
	_surface = IMG_Load("data/graphics/" _filename);\
	if (_surface == NULL)\
	{\
		return false;\
	}
	LOAD_SURFACE(bg->Layer0, "bg0.png");
	LOAD_SURFACE(bg->Layer1, "bg1.png");
	LOAD_SURFACE(bg->Layer2, "bg2.png");
	LOAD_SURFACE(bg->Layer3, "bg3.png");
	return true;
}
void BackgroundsFree(Backgrounds* bg)
{
	SDL_FreeSurface(bg->Layer0);
	SDL_FreeSurface(bg->Layer1);
	SDL_FreeSurface(bg->Layer2);
	SDL_FreeSurface(bg->Layer3);
}
