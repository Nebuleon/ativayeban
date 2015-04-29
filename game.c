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
#include "platform.h"
#include "player.h"
#include "sound.h"
#include "space.h"
#include "utils.h"
#include "game.h"
#include "gap.h"
#include "score.h"
#include "draw.h"
#include "bg.h"

static uint32_t               Score;
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
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		if (!players[i].Enabled) continue;
		players[i].AccelX = GetMovement(i);
	}
}

static void AddEdgeShapes(const float y);

static void ScoreGaps();
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
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		Player *p = &players[i];
		if (!p->Enabled) continue;
		PlayerUpdate(p);
		hasPlayers = true;

		// Players that hit the top of the screen die
		if (cpBodyGetPosition(p->Body).y + PLAYER_RADIUS >=
			camera.Y + FIELD_HEIGHT / 2)
		{
			PlayerDisable(p);
		}
	}
	// If no players left alive, end the game
	if (!hasPlayers)
	{
		ToScore(Score);
	}
	SpaceUpdate(&space, PlayerMinY(), camera.Y, PlayerMaxY(), &Score);

	// Players that hit the top of the screen die
	if (PlayerMaxY() + PLAYER_RADIUS >= camera.Y + FIELD_HEIGHT / 2)
	{
		ToScore(Score);
	}
}
static float PlayerMiddleY(void)
{
	float sum = 0;
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		const Player *p = &players[i];
		if (!p->Enabled) continue;
		sum += (float)cpBodyGetPosition(p->Body).y;
	}
	return sum / PlayerEnabledCount();
}
static float PlayerMinY(void)
{
	float y = NAN;
	for (int i = 0; i < MAX_PLAYERS; i++)
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
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		const Player *p = &players[i];
		if (!p->Enabled) continue;
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
		MAX(-SCREEN_HEIGHT, SCREEN_Y(camera.Y) - SCREEN_HEIGHT / 2);
	// Draw the background.
	DrawBackground(&BG, screenYOff);

	SpaceDraw(&space, screenYOff);

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		if (!players[i].Enabled) continue;
		PlayerDraw(&players[i], screenYOff);
	}

	// Draw the player's current score.
	char ScoreString[17];
	sprintf(ScoreString, "Score%10" PRIu32, Score);
	SDL_Color white = { 255, 255, 255, 255 };
	SDL_Surface *t = TTF_RenderText_Blended(font, ScoreString, white);
	SDL_Rect dest = { (Sint16)(SCREEN_WIDTH - t->w), 0, 0, 0 };
	SDL_BlitSurface(t, NULL, Screen, &dest);

	SDL_Flip(Screen);
	SDL_FreeSurface(t);
}

void ToGame(void)
{
	Score = 0;
	Pause = false;

	SpaceReset(&space);

	// Reset player positions and velocity
	// TODO: continue from title screen
	for (int i = 0, c = 0; i < MAX_PLAYERS; i++)
	{
		if (!players[i].Enabled) continue;
		cpBody *body = players[i].Body;
		cpBodySetPosition(body, cpv(
			(c + 1) * FIELD_WIDTH / (PlayerEnabledCount() + 1),
			FIELD_HEIGHT * 0.75f));
		cpBodySetVelocity(body, cpvzero);
		c++;
	}
	CameraInit(&camera);
	SoundPlay(SoundStart, 1.0);
	MusicSetLoud(true);

	GatherInput = GameGatherInput;
	DoLogic     = GameDoLogic;
	OutputFrame = GameOutputFrame;
}
