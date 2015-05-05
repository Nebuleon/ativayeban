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
#include "particle.h"

#include <math.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288
#endif
#include <stdbool.h>

#include "game.h"


typedef struct
{
	Animation anim;	// note: no ownership, don't free
	float x;
	float y;
	float dx;
	float dy;
} Particle;

CArray Particles;


void ParticlesInit(void)
{
	CArrayInit(&Particles, sizeof(Particle));
}
void ParticlesFree(void)
{
	CArrayTerminate(&Particles);
}

void ParticlesAdd(
	const Animation *anim, const float x, const float y,
	const float dx, const float dy)
{
	Particle p;
	memset(&p, 0, sizeof p);
	memcpy(&p.anim, anim, sizeof *anim);
	p.x = x;
	p.y = y;
	p.dx = dx;
	p.dy = dy;
	CArrayPushBack(&Particles, &p);
}
void ParticlesAddExplosion(
	const Animation *anim, const float x, const float y, const int n,
	const float speed)
{
	for (int i = 0; i < n; i++)
	{
		const float theta = (float)rand() / RAND_MAX * (float)M_PI * 2;
		ParticlesAdd(
			anim, x, y, (float)cos(theta) * speed, (float)sin(theta) * speed);
	}
}

static bool ParticleUpdate(Particle *p, const Uint32 ms);
void ParticlesUpdate(const Uint32 ms)
{
	for (int i = 0; i < (int)Particles.size; i++)
	{
		Particle *p = CArrayGet(&Particles, i);
		if (!ParticleUpdate(p, ms))
		{
			CArrayDelete(&Particles, i);
		}
	}
}
// Return whether to keep the particle
static bool ParticleUpdate(Particle *p, const Uint32 ms)
{
	p->x += p->dx * ms / 1000;
	p->y += p->dy * ms / 1000;
	return !AnimationUpdate(&p->anim, ms);
}
static void ParticleDraw(const Particle *p, SDL_Surface *screen, const float y);
void ParticlesDraw(SDL_Surface *screen, const float y)
{
	for (int i = 0; i < (int)Particles.size; i++)
	{
		ParticleDraw(CArrayGet(&Particles, i), screen, y);
	}
}
static void ParticleDraw(const Particle *p, SDL_Surface *screen, const float y)
{
	AnimationDraw(
		&p->anim, screen, (int)SCREEN_X(p->x), (int)(SCREEN_Y(p->y) - y));
}
