/*
 * Ativayeban, game code file
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

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <math.h>

#include <SDL.h>

#include "camera.h"
#include "main.h"
#include "init.h"
#include "input.h"
#include "particle.h"
#include "pickup.h"
#include "platform.h"
#include "player.h"
#include "sound.h"
#include "space.h"
#include "title.h"
#include "utils.h"
#include "game.h"
#include "gap.h"
#include "draw.h"
#include "bg.h"

static bool                   Pause;

Mix_Chunk* SoundBeep = NULL;
Mix_Chunk* SoundStart = NULL;
Mix_Chunk* SoundLose = NULL;
Mix_Chunk* SoundScore = NULL;

TTF_Font *font = NULL;


void GameGatherInput(bool* Continue)
{
	SDL_Event ev;

	while (SDL_PollEvent(&ev))
	{
		InputOnEvent(&ev);
		if (IsPauseEvent(&ev))
			Pause = !Pause;
		else if (IsExitGameEvent(&ev))
		{
			*Continue = false;
			return;
		}
	}
	for (int i = 0; i < NumPlayers; i++)
	{
		if (!players[i].Alive) continue;
		players[i].AccelX = GetMovement(i);
	}
}

static float PlayerMiddleY(void);
static float PlayerMinY(void);
static float PlayerMaxY(void);
void GameDoLogic(bool* Continue, bool* Error, Uint32 Milliseconds)
{
	(void)Continue;
	(void)Error;
	if (Pause) return;

	cpSpaceStep(space.Space, Milliseconds * 0.001);
	CameraUpdate(&camera, PlayerMiddleY(), Milliseconds);

	bool hasPlayers = false;
	for (int i = 0; i < NumPlayers; i++)
	{
		Player *p = &players[i];
		if (!p->Enabled) continue;
		PlayerUpdate(p, Milliseconds);
		// Check if the player needs to be respawned
		if (p->RespawnCounter == 0 && !p->Alive && space.Gaps.size > 0)
		{
			SpaceRespawnPlayer(&space, p);
		}
		if (!p->Alive)
		{
			// Check if any players are past ones that await reenabling
			if (p->RespawnCounter == -1 && PlayerMinY() < p->y)
			{
				PlayerRevive(p);
			}
			else
			{
				continue;
			}
		}
		hasPlayers = true;

		// Check player pickups
		if (PickupsCollide(p->x, p->y, PLAYER_RADIUS))
		{
			PlayerScore(p, false);
		}

		// Players that hit the top of the screen die
		if (cpBodyGetPosition(p->Body).y + PLAYER_RADIUS >=
			camera.Y + FIELD_HEIGHT / 2)
		{
			PlayerKill(p);
		}
	}
	// If no players left alive, end the game
	if (!hasPlayers)
	{
		ToTitleScreen(false);
	}
	SpaceUpdate(&space, PlayerMinY(), camera.Y, PlayerMaxY(), &players[0]);

	ParticlesUpdate(Milliseconds);

	// Players that hit the top of the screen die
	if (PlayerMaxY() + PLAYER_RADIUS >= camera.Y + FIELD_HEIGHT / 2)
	{
		ToTitleScreen(false);
	}
}
static float PlayerMiddleY(void)
{
	float sum = 0;
	for (int i = 0; i < NumPlayers; i++)
	{
		const Player *p = &players[i];
		if (!p->Alive) continue;
		sum += (float)cpBodyGetPosition(p->Body).y;
	}
	return sum / PlayerAliveCount();
}
static float PlayerMinY(void)
{
	float y = NAN;
	for (int i = 0; i < NumPlayers; i++)
	{
		const Player *p = &players[i];
		if (!p->Enabled) continue;
		const float py = (float)cpBodyGetPosition(p->Body).y;
		if (isnan(y) || py < y)
		{
			y = py;
		}
	}
	return y;
}
static float PlayerMaxY(void)
{
	float y = NAN;
	for (int i = 0; i < NumPlayers; i++)
	{
		const Player *p = &players[i];
		if (!p->Alive) continue;
		const float py = (float)cpBodyGetPosition(p->Body).y;
		if (isnan(y) || py > y)
		{
			y = py;
		}
	}
	return y;
}

void GameOutputFrame(void)
{
	const float screenYOff =
		(float)MAX(-SCREEN_HEIGHT, SCREEN_Y(camera.Y) - SCREEN_HEIGHT / 2);
	// Draw the background.
	DrawBackground(&BG, screenYOff);

	SpaceDraw(&space, screenYOff);
	PickupsDraw(screenYOff);
	ParticlesDraw(screenYOff);

	int c = 0;
	for (int i = 0; i < NumPlayers; i++)
	{
		PlayerDraw(&players[i], screenYOff);

		if (!players[i].Enabled) continue;

		// Draw each player's current score.
		char buf[17];
		sprintf(buf, "%d", players[i].Score);
		const SDL_Color white = { 255, 255, 255, 255 };
		Tex t = LoadText(buf, font, white);
		if (t.T == NULL) continue;
		const int x = (c + 1) * SCREEN_WIDTH / (PlayerEnabledCount() + 1);
		const int wHalf = (t.W + PLAYER_SPRITESHEET_WIDTH) / 2;
		// Draw the player icon, followed by the score number
		SDL_Rect src = {
			0, 0, PLAYER_SPRITESHEET_WIDTH, PLAYER_SPRITESHEET_HEIGHT
		};
		SDL_Rect dest = { x - wHalf, 0, src.w, src.h };
		RenderTex(PlayerSpritesheets[i].T, &src, &dest);
		// Draw score number
		dest.x = (Sint16)(x - wHalf + PLAYER_SPRITESHEET_WIDTH);
		dest.y = (Sint16)(PLAYER_SPRITESHEET_HEIGHT - t.H) / 2;
		dest.w = t.W;
		dest.h = t.H;
		RenderTex(t.T, NULL, &dest);
		SDL_DestroyTexture(t.T);

		c++;
	}

	SDL_RenderPresent(Renderer);
}

void ToGame(void)
{
	Pause = false;

	SpaceReset(&space);

	// Reset player positions and velocity
	for (int i = 0, c = 0; i < NumPlayers; i++)
	{
		PlayerReset(&players[i], c);
		if (!players[i].Enabled) continue;
		c++;
	}
	CameraInit(&camera);
	SoundPlay(SoundStart, 1.0);
	MusicSetLoud(true);

	GatherInput = GameGatherInput;
	DoLogic     = GameDoLogic;
	OutputFrame = GameOutputFrame;
}
