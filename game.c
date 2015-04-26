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

static bool                   Boost;
static bool                   Pause;

// What the player avoids.
static struct Gap*            Gaps     = NULL;
static uint32_t               GapCount = 0;

static float                  GenDistance;

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

	ScoreGaps();

	// Generate a gap now if needed.
	if (GapCount == 0 || GapBottom(&Gaps[GapCount - 1]) - (camera.Y - FIELD_HEIGHT / 2) >= GenDistance)
	{
		float Top;
		if (GapCount == 0)
			Top = 0;
		else
		{
			Top = GapBottom(&Gaps[GapCount - 1]) - GenDistance;
			GenDistance += GAP_GEN_SPEED;
			GenDistance = MAX(GAP_GEN_MIN, GenDistance);
		}
		Gaps = realloc(Gaps, (GapCount + 1) * sizeof(struct Gap));
		GapCount++;
		// Where's the place for the player to go through?
		float GapLeft = (FIELD_WIDTH / 16.0f) + ((float) rand() / (float) RAND_MAX) * (FIELD_WIDTH - GAP_WIDTH - (FIELD_WIDTH / 16.0f));
		GapInit(&Gaps[GapCount - 1], Top, GapLeft);
	}

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		PlayerUpdate(&players[i]);
	}
	CameraUpdate(&camera, PlayerMiddleY(), Milliseconds);
	SpaceUpdate(&space, PlayerMinY());

	// If the ball has collided with the top of the field,
	// the player's game is over.
	if (PlayerMaxY() + PLAYER_RADIUS >= camera.Y + FIELD_HEIGHT / 2)
	{
		ToScore(Score);
	}
}
static void ScoreGaps()
{
	// Scroll all gaps toward the top...
	for (int i = GapCount - 1; i >= 0; i--)
	{
		// If the player is past a gap, award the player with a
		// point.
		if (!Gaps[i].Passed &&
			Gaps[i].Y > PlayerMaxY() + PLAYER_RADIUS)
		{
			Gaps[i].Passed = true;
			Score++;
			SoundPlay(SoundScore, 1.0);
		}
		// Arbitrary limit to eliminate off screen gaps
		// If a gap is past the top side, remove it.
		if (GapBottom(&Gaps[i]) > PlayerMaxY() + FIELD_HEIGHT * 2)
		{
			GapRemove(&Gaps[i]);
			memmove(&Gaps[i], &Gaps[i + 1], (GapCount - i) * sizeof(struct Gap));
			GapCount--;
		}
	}
}
static float PlayerMiddleY(void)
{
	float sum = 0;
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		sum += players[i].Y;
	}
	return sum / MAX_PLAYERS;
}
static float PlayerMinY(void)
{
	float y = players[0].Y;
	for (int i = 1; i < MAX_PLAYERS; i++)
	{
		y = min(y, players[i].Y);
	}
	return y;
}
static float PlayerMaxY(void)
{
	float y = players[0].Y;
	for (int i = 1; i < MAX_PLAYERS; i++)
	{
		y = max(y, players[i].Y);
	}
	return y;
}

void GameOutputFrame(void)
{
	const float screenYOff = MAX(0.0f, SCREEN_Y(camera.Y) - SCREEN_HEIGHT / 2);
	// Draw the background.
	DrawBackground(&BG, screenYOff);

	// Draw the gaps.
	uint32_t i;
	for (i = 0; i < GapCount; i++)
	{
		GapDraw(&Gaps[i], screenYOff);
	}

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
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
	Boost = false;
	Pause = false;

	SpaceReset(&space);

	// Reset player positions
	// TODO: continue from title screen
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		cpBodySetPosition(players[i].Body, cpv(
			(i + 1) * FIELD_WIDTH / (MAX_PLAYERS + 1),
			FIELD_HEIGHT * 0.75f));
	}
	if (Gaps != NULL)
	{
		free(Gaps);
		Gaps = NULL;
	}
	GapCount = 0;
	GenDistance = GAP_GEN_START;
	CameraInit(&camera);
	SoundPlay(SoundStart, 1.0);
	MusicSetLoud(true);

	GatherInput = GameGatherInput;
	DoLogic     = GameDoLogic;
	OutputFrame = GameOutputFrame;
}
