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

#include "SDL.h"
#include "SDL_image.h"

#include "main.h"
#include "init.h"
#include "platform.h"
#include "player.h"
#include "sound.h"
#include "game.h"
#include "gap.h"
#include "score.h"
#include "draw.h"
#include "bg.h"
#include "text.h"

static uint32_t               Score;

static bool                   Boost;
static bool                   Pause;

static struct Player          Player;

// What the player avoids.
static struct Gap*            Gaps     = NULL;
static uint32_t               GapCount = 0;

static float                  GenDistance;

void GameGatherInput(bool* Continue)
{
	SDL_Event ev;

	while (SDL_PollEvent(&ev))
	{
		Player.AccelX = GetMovement(&ev);
		if (IsPauseEvent(&ev))
			Pause = !Pause;
		else if (IsExitGameEvent(&ev))
		{
			*Continue = false;
			return;
		}
	}
}

static void AnimationControl(Uint32 Milliseconds)
{
	(void)Milliseconds;
}

void GameDoLogic(bool* Continue, bool* Error, Uint32 Milliseconds)
{
	(void)Continue;
	(void)Error;
	if (!Pause)
	{
		bool PointAwarded = false;
		uint32_t Millisecond;
		for (Millisecond = 0; Millisecond < Milliseconds; Millisecond++)
		{
			// Scroll all gaps toward the top...
			int32_t i;
			for (i = GapCount - 1; i >= 0; i--)
			{
				bool PastTop = GapUpdate(&Gaps[i], FIELD_SCROLL / 1000);
				// If the player is past a gap, award the player with a
				// point. But there is a pair of them per row!
				if (!Gaps[i].Passed
				 && Gaps[i].Y > Player.Y + PLAYER_SIZE / 2)
				{
					Gaps[i].Passed = true;
					if (!PointAwarded)
					{
						Score++;
						PointAwarded = true;
					}
				}
				// If a gap is past the top side, remove it.
				if (PastTop)
				{
					memmove(&Gaps[i], &Gaps[i + 1], (GapCount - i) * sizeof(struct Gap));
					GapCount--;
				}
			}
			// Generate a gap now if needed.
			if (GapCount == 0 || GapBottom(&Gaps[GapCount - 1]) >= GenDistance)
			{
				float Top;
				if (GapCount == 0)
					Top = FIELD_SCROLL / 1000;
				else
				{
					Top = GapBottom(&Gaps[GapCount - 1]) - GenDistance;
					GenDistance += GAP_GEN_SPEED;
				}
				Gaps = realloc(Gaps, (GapCount + 1) * sizeof(struct Gap));
				GapCount++;
				// Where's the place for the player to go through?
				float GapLeft = (FIELD_WIDTH / 16.0f) + ((float) rand() / (float) RAND_MAX) * (FIELD_WIDTH - GAP_WIDTH - (FIELD_WIDTH / 16.0f));
				GapInit(&Gaps[GapCount - 1], Top, GapLeft);
			}
			
			PlayerUpdate(&Player);

			// Is the ball on a gap?
			bool OnGap = false;

			for (i = GapCount - 1; i >= 0; i--)
			{
				// Stop considering gaps if they are higher than the ball
				// (Y).
				if (Player.Y - PLAYER_SIZE / 2 < Gaps[i].Y + LOWER_EPSILON)
					break;
				// If the ball is in range (X)...
				if (GapIsOn(&Gaps[i], Player.X, PLAYER_SIZE / 2))
				{
					// Is the distance in range, and is the ball going up slowly
					// enough (Y)?
					if (Player.Y - PLAYER_SIZE / 2 <  Gaps[i].Y + UPPER_EPSILON
					 && Player.SpeedY              >= 0.0f
					 && Player.SpeedY              <  UPPER_EPSILON)
					{
						OnGap = true;
						Player.SpeedY = 0.0f;
						// Also make sure the ball appears to be on the gap (Y).
						Player.Y = Gaps[i].Y + PLAYER_SIZE / 2;
						break;
					}
					// If the ball would cross the gap during this
					// millisecond, make it rebound instead (Y).
					else if (Player.Y - PLAYER_SIZE / 2                        >= Gaps[i].Y
					      && Player.Y - PLAYER_SIZE / 2 + Player.SpeedY / 1000 <  Gaps[i].Y)
					{
						Player.Y = Player.Y
							+ ((Player.Y - PLAYER_SIZE / 2) - Gaps[i].Y - (Player.SpeedY / 1000)) * GAP_REBOUND;
						Player.SpeedY = -Player.SpeedY * GAP_REBOUND;
						SoundPlayBounce(Player.SpeedY);
						break;
					}
				}
			}

			if (OnGap)
			{
				// If so, apply friction to the player's speed (X).
				Player.SpeedX *= 1.0f - FRICTION;
			}
			else
			{
				// If not, apply gravity (Y).
				Player.SpeedY += GRAVITY / 1000;
				Player.Y += Player.SpeedY / 1000;
			}

			// Bottom edge (Y).
			// If the ball arrives below the bottom edge, act as if it had
			// arrived on the bottom edge.
			if (Player.Y - PLAYER_SIZE / 2 < 0)
			{
				Player.Y = PLAYER_SIZE / 2;
				Player.SpeedY = -Player.SpeedY * FIELD_REBOUND;
				SoundPlayBounce(Player.SpeedY);
			}

			// If the ball has collided with the top of the field,
			// and it is not going down, the player's game is over.
			if (Player.Y + PLAYER_SIZE / 2 >= FIELD_HEIGHT
			 && Player.SpeedY              >= 0.0f)
			{
				ToScore(Score);
				break;
			}
		}

		AdvanceBackground(Milliseconds);
	}

	AnimationControl(Milliseconds);
}

void GameOutputFrame(void)
{
	// Draw the background.
	DrawBackground();

	// Draw the gaps.
	uint32_t i;
	for (i = 0; i < GapCount; i++)
	{
		GapDraw(&Gaps[i]);
	}

	PlayerDraw(&Player);

	// Draw the player's current score.
	char ScoreString[17];
	sprintf(ScoreString, "Score%10" PRIu32, Score);
	if (SDL_MUSTLOCK(Screen))
		SDL_LockSurface(Screen);
	PrintStringOutline32(ScoreString,
		SDL_MapRGB(Screen->format, 255, 255, 255),
		SDL_MapRGB(Screen->format, 0, 0, 0),
		Screen->pixels,
		Screen->pitch,
		0,
		0,
		SCREEN_WIDTH,
		SCREEN_HEIGHT,
		RIGHT,
		TOP);
	if (SDL_MUSTLOCK(Screen))
		SDL_UnlockSurface(Screen);

	SDL_Flip(Screen);
}

void ToGame(void)
{
	Score = 0;
	Boost = false;
	Pause = false;
	PlayerReset(&Player);
	if (Gaps != NULL)
	{
		free(Gaps);
		Gaps = NULL;
	}
	GapCount = 0;
	GenDistance = GAP_GEN_START;

	GatherInput = GameGatherInput;
	DoLogic     = GameDoLogic;
	OutputFrame = GameOutputFrame;
}
