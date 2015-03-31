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
#include "platform.h"
#include "player.h"
#include "sound.h"
#include "utils.h"
#include "game.h"
#include "gap.h"
#include "score.h"
#include "draw.h"
#include "bg.h"

static uint32_t               Score;

static bool                   Boost;
static bool                   Pause;

static Player                 player;

// What the player avoids.
static struct Gap*            Gaps     = NULL;
static uint32_t               GapCount = 0;

static float                  GenDistance;

static cpBody*                edgeBodies = NULL;
static float                  edgeBodiesBottom = -FIELD_HEIGHT * 4;

Mix_Chunk* SoundStart = NULL;
Mix_Chunk* SoundLose = NULL;
Mix_Chunk* SoundScore = NULL;

TTF_Font *font = NULL;

cpSpace *Space = NULL;


void GameGatherInput(bool* Continue)
{
	SDL_Event ev;

	while (SDL_PollEvent(&ev))
	{
		player.AccelX = GetMovement(&ev);
		if (IsPauseEvent(&ev))
			Pause = !Pause;
		else if (IsExitGameEvent(&ev))
		{
			*Continue = false;
			return;
		}
	}
}

static void AddEdgeShapes(const float y);

static void ScoreGaps();
static void RemoveEdgeShape(cpBody *body, cpShape *shape, void *data);
void GameDoLogic(bool* Continue, bool* Error, Uint32 Milliseconds)
{
	(void)Continue;
	(void)Error;
	if (Pause) return;

	cpSpaceStep(Space, Milliseconds * 0.001);

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
			GenDistance = max(GAP_GEN_MIN, GenDistance);
		}
		Gaps = realloc(Gaps, (GapCount + 1) * sizeof(struct Gap));
		GapCount++;
		// Where's the place for the player to go through?
		float GapLeft = (FIELD_WIDTH / 16.0f) + ((float) rand() / (float) RAND_MAX) * (FIELD_WIDTH - GAP_WIDTH - (FIELD_WIDTH / 16.0f));
		GapInit(&Gaps[GapCount - 1], Top, GapLeft);
	}

	PlayerUpdate(&player);
	CameraUpdate(&camera, &player, Milliseconds);
	if (player.Y < edgeBodiesBottom)
	{
		cpBodyEachShape(edgeBodies, RemoveEdgeShape, NULL);
		AddEdgeShapes(player.Y);
	}

	// If the ball has collided with the top of the field,
	// the player's game is over.
	if (player.Y + PLAYER_RADIUS >= camera.Y + FIELD_HEIGHT / 2)
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
			Gaps[i].Y > player.Y + PLAYER_RADIUS)
		{
			Gaps[i].Passed = true;
			Score++;
			SoundPlay(SoundScore, 1.0);
		}
		// Arbitrary limit to eliminate off screen gaps
		// If a gap is past the top side, remove it.
		if (GapBottom(&Gaps[i]) > player.Y + FIELD_HEIGHT * 2)
		{
			GapRemove(&Gaps[i]);
			memmove(&Gaps[i], &Gaps[i + 1], (GapCount - i) * sizeof(struct Gap));
			GapCount--;
		}
	}
}
static void RemoveEdgeShape(cpBody *body, cpShape *shape, void *data)
{
	UNUSED(body);
	UNUSED(data);
	//printf("Remove edge shape\n");
	cpSpaceRemoveShape(Space, shape);
	cpShapeFree(shape);
}

void GameOutputFrame(void)
{
	const float screenYOff = max(0.0f, SCREEN_Y(camera.Y) - SCREEN_HEIGHT / 2);
	// Draw the background.
	DrawBackground(&BG, screenYOff);

	// Draw the gaps.
	uint32_t i;
	for (i = 0; i < GapCount; i++)
	{
		GapDraw(&Gaps[i], screenYOff);
	}

	PlayerDraw(&player, screenYOff);

	// Draw the player's current score.
	char ScoreString[17];
	sprintf(ScoreString, "Score%10" PRIu32, Score);
	SDL_Color white = { 255, 255, 255, 255 };
	SDL_Surface *t = TTF_RenderText_Blended(font, ScoreString, white);
	SDL_Rect dest = { (Sint16)(SCREEN_WIDTH - t->w), 0, 0, 0 };
	if (SDL_MUSTLOCK(Screen))
		SDL_LockSurface(Screen);
	SDL_BlitSurface(t, NULL, Screen, &dest);
	if (SDL_MUSTLOCK(Screen))
		SDL_UnlockSurface(Screen);

	SDL_Flip(Screen);
	SDL_FreeSurface(t);
}

void ToGame(void)
{
	Score = 0;
	Boost = false;
	Pause = false;
	cpSpaceFree(Space);

	// Init physics space
	Space = cpSpaceNew();
	cpSpaceSetIterations(Space, 30);
	cpSpaceSetGravity(Space, cpv(0, GRAVITY));
	cpSpaceSetCollisionSlop(Space, 0.5);
	cpSpaceSetSleepTimeThreshold(Space, 1.0f);
	// Segments around screen
	edgeBodies = cpSpaceGetStaticBody(Space);
	AddEdgeShapes(0);

	PlayerInit(&player);
	if (Gaps != NULL)
	{
		free(Gaps);
		Gaps = NULL;
	}
	GapCount = 0;
	GenDistance = GAP_GEN_START;
	BackgroundsInit(&BG);
	CameraInit(&camera);
	SoundPlay(SoundStart, 1.0);

	GatherInput = GameGatherInput;
	DoLogic     = GameDoLogic;
	OutputFrame = GameOutputFrame;
}

static void AddEdgeShapes(const float y)
{
	//printf("Add edge shapes from %f\n", y);
	cpShape *shape;
	cpShapeFilter edgeFilter = { CP_NO_GROUP, CP_ALL_CATEGORIES, CP_ALL_CATEGORIES };
	const float top = y + FIELD_HEIGHT * 2;
	// Left edge
	edgeBodiesBottom = y - FIELD_HEIGHT * 4;
	shape = cpSpaceAddShape(Space, cpSegmentShapeNew(
		edgeBodies, cpv(0, edgeBodiesBottom), cpv(0, top), 0.0f));
	cpShapeSetElasticity(shape, 1.0f);
	cpShapeSetFriction(shape, 1.0f);
	cpShapeSetFilter(shape, edgeFilter);
	// Right edge
	shape = cpSpaceAddShape(Space, cpSegmentShapeNew(
		edgeBodies, cpv(FIELD_WIDTH, edgeBodiesBottom), cpv(FIELD_WIDTH, top), 0.0f));
	cpShapeSetElasticity(shape, 1.0f);
	cpShapeSetFriction(shape, 1.0f);
	cpShapeSetFilter(shape, edgeFilter);
}
