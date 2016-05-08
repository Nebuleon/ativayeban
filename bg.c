/*
 * Copyright (C) 2014 Nebuleon Fumika <nebuleon@gcw-zero.com>
 * 2015 Cong Xu <congusbongus@gmail.com>
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

#include <stdlib.h>
#include <string.h>

#include <SDL_image.h>

#include "draw.h"
#include "init.h"
#include "main.h"
#include "sys_config.h"
#include "utils.h"

#define ICICLE_WIDTH 58
#define ICICLE_HEIGHT 77
#define ICICLE_Y_GAP_MIN 60
#define ICICLE_Y_GAP_MAX 120
#define ICICYLE_NUM 6
#define FLARE_WIDTH 12
#define FLARE_HEIGHT 12
#define FLARE_Y_GAP_MIN 20
#define FLARE_Y_GAP_MAX 40
#define FLARE_NUM 4
#define STAR_WIDTH 4
#define STAR_HEIGHT 4
#define STAR_Y_GAP_MIN 0
#define STAR_Y_GAP_MAX 8
#define STAR_NUM 4
#define PARTICLE_RAND_X(_w) (-(_w) + (rand() % ((_w) + SCREEN_WIDTH)))
#define PARTICLE_RAND_Y(_y, _gmin, _gmax) ((_y) + (_gmin) + (rand() % ((_gmax) - (_gmin))))
#define PARTICLE_RAND_INDEX(_num) (rand() % (_num))

#define SCROLL_FACTOR 0.1f
#define SCALE_1 1.0f
#define SCALE_2 2.0f
#define SCALE_3 3.0f

Backgrounds BG;

static void ParticlesInit(
	BGParticles *p,
	const int w, const int gmin, const int gmax, const int num)
{
	int y = PARTICLE_RAND_Y(0, gmin, gmax);
	for (int i = 0; i < MAX_PARTICLES; i++)
	{
		// Generate some random particles
		p->Positions[i].X = PARTICLE_RAND_X(w);
		p->Positions[i].Y = PARTICLE_RAND_Y(y, gmin, gmax);
		p->Positions[i].Index = PARTICLE_RAND_INDEX(num);
		y = p->Positions[i].Y;
	}
}

void BackgroundsInit(Backgrounds *bg)
{
	ParticlesInit(
		&bg->Icicles,
		ICICLE_WIDTH, ICICLE_Y_GAP_MIN, ICICLE_Y_GAP_MAX, ICICYLE_NUM);
	ParticlesInit(
		&bg->Flares,
		FLARE_WIDTH, FLARE_Y_GAP_MIN, FLARE_Y_GAP_MAX, FLARE_NUM);
	ParticlesInit(
		&bg->Stars,
		STAR_WIDTH, STAR_Y_GAP_MIN, STAR_Y_GAP_MAX, STAR_NUM);
}

static void DrawParticleScroll(
	BGParticles *p, const int s,
	const int w, const int h, const int gmin, const int gmax, const int num);
void DrawBackground(Backgrounds *bg, const float y)
{
	SDL_SetRenderDrawColor(Renderer, 8, 3, 32, 255);
	SDL_RenderClear(Renderer);
	DrawParticleScroll(
		&bg->Icicles, (int)(y * SCROLL_FACTOR * SCALE_1),
		ICICLE_WIDTH, ICICLE_HEIGHT,
		ICICLE_Y_GAP_MIN, ICICLE_Y_GAP_MAX, ICICYLE_NUM);
	DrawParticleScroll(
		&bg->Flares, (int)(y * SCROLL_FACTOR * SCALE_2),
		FLARE_WIDTH, FLARE_HEIGHT,
		FLARE_Y_GAP_MIN, FLARE_Y_GAP_MAX, FLARE_NUM);
	DrawParticleScroll(
		&bg->Stars, (int)(y * SCROLL_FACTOR * SCALE_3),
		STAR_WIDTH, STAR_HEIGHT,
		STAR_Y_GAP_MIN, STAR_Y_GAP_MAX, STAR_NUM);
}
static void DrawParticleScroll(
	BGParticles *p, const int s,
	const int w, const int h, const int gmin, const int gmax, const int num)
{
	// Remove, draw or add icicles
	int lastY = -1;
	for (int i = 0; i < MAX_PARTICLES; i++)
	{
		if (p->Positions[i].Index == -1)
		{
			// No icicles past this point; generate a new one in its place
			p->Positions[i].X = PARTICLE_RAND_X(w);
			p->Positions[i].Y = PARTICLE_RAND_Y(lastY, gmin, gmax);
			if (p->Positions[i].Y < s + SCREEN_HEIGHT)
			{
				// Always spawn past the bottom
				p->Positions[i].Y = s + SCREEN_HEIGHT;
			}
			p->Positions[i].Index = PARTICLE_RAND_INDEX(num);
		}
		else if (p->Positions[i].Y < s - h)
		{
			// Icicle past screen top, shuffle items forward
			memmove(
				&p->Positions[i],
				&p->Positions[i + 1],
				sizeof p->Positions[i] * (MAX_PARTICLES - i - 1));
			// Make sure to initialise a new one at the end
			if (i < MAX_PARTICLES - 1)
			{
				p->Positions[MAX_PARTICLES - 1].Index = -1;
			}
			i--;
			continue;
		}

		// Draw if in range
		if (p->Positions[i].Y < s + SCREEN_HEIGHT)
		{
			SDL_Rect src = { p->Positions[i].Index * w, 0, w, h };
			SDL_Rect dst = {
				p->Positions[i].X, p->Positions[i].Y - s, src.w, src.h
			};
			RenderTex(p->T.T, &src, &dst);
		}

		lastY = p->Positions[i].Y;
	}
}

bool BackgroundsLoad(Backgrounds* bg)
{
#define LOAD_TEX(_t, _filename)\
	_t = LoadTex(DATA_DIR "graphics/" _filename);\
	if (_t.T == NULL)\
	{\
		return false;\
	}
	LOAD_TEX(bg->Icicles.T, "icebergs.png");
	LOAD_TEX(bg->Flares.T, "flare.png");
	LOAD_TEX(bg->Stars.T, "stars.png");
	return true;
}
void BackgroundsFree(Backgrounds* bg)
{
	SDL_DestroyTexture(bg->Icicles.T.T);
	SDL_DestroyTexture(bg->Flares.T.T);
	SDL_DestroyTexture(bg->Stars.T.T);
}
