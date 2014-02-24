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
#include "game.h"
#include "score.h"
#include "draw.h"
#include "bg.h"
#include "text.h"

static uint32_t               Score;

static bool                   Boost;
static bool                   Pause;

// Where the player is. (Center, meters.)
static float                  PlayerX;
static float                  PlayerY;
// Where the player is going. (Meters per second.)
static float                  PlayerSpeedX;
static float                  PlayerSpeedY;

// The last value returned by GetMovement.
static int16_t                PlayerAccelX;

// What the player avoids.
static struct AtivayebanRect* Rectangles     = NULL;
static uint32_t               RectangleCount = 0;

static float                  GenDistance;

void GameGatherInput(bool* Continue)
{
	SDL_Event ev;

	while (SDL_PollEvent(&ev))
	{
		PlayerAccelX = GetMovement(&ev);
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
}

void GameDoLogic(bool* Continue, bool* Error, Uint32 Milliseconds)
{
	if (!Pause)
	{
		bool PointAwarded = false;
		uint32_t Millisecond;
		for (Millisecond = 0; Millisecond < Milliseconds; Millisecond++)
		{
			// Scroll all rectangles toward the top...
			int32_t i;
			for (i = RectangleCount - 1; i >= 0; i--)
			{
				Rectangles[i].Top += FIELD_SCROLL / 1000;
				Rectangles[i].Bottom += FIELD_SCROLL / 1000;
				// If the player is past a rectangle, award the player with a
				// point. But there is a pair of them per row!
				if (!Rectangles[i].Passed
				 && Rectangles[i].Top > PlayerY + PLAYER_SIZE / 2)
				{
					Rectangles[i].Passed = true;
					if (!PointAwarded)
					{
						Score++;
						PointAwarded = true;
					}
				}
				// If a rectangle is past the top side, remove it.
				if (Rectangles[i].Bottom > FIELD_HEIGHT)
				{
					memmove(&Rectangles[i], &Rectangles[i + 1], (RectangleCount - i) * sizeof(struct AtivayebanRect));
					RectangleCount--;
				}
			}
			// ... as well as the ball.
			PlayerY += FIELD_SCROLL / 1000;
			// Generate a pair of rectangles now if needed.
			if (RectangleCount == 0 || Rectangles[RectangleCount - 1].Bottom >= GenDistance)
			{
				float Top;
				if (RectangleCount == 0)
					Top = FIELD_SCROLL / 1000;
				else
				{
					Top = Rectangles[RectangleCount - 1].Bottom - GenDistance;
					GenDistance += RECT_GEN_SPEED;
				}
				Rectangles = realloc(Rectangles, (RectangleCount + 2) * sizeof(struct AtivayebanRect));
				RectangleCount += 2;
				Rectangles[RectangleCount - 2].Passed = Rectangles[RectangleCount - 1].Passed = false;
				Rectangles[RectangleCount - 2].Top = Rectangles[RectangleCount - 1].Top = Top;
				Rectangles[RectangleCount - 2].Bottom = Rectangles[RectangleCount - 1].Bottom = Top - RECT_HEIGHT;
				// Where's the place for the player to go through?
				float GapLeft = (FIELD_WIDTH / 16.0f) + ((float) rand() / (float) RAND_MAX) * (FIELD_WIDTH - GAP_WIDTH - (FIELD_WIDTH / 16.0f));
				Rectangles[RectangleCount - 2].Left = 0;
				Rectangles[RectangleCount - 2].Right = GapLeft;
				Rectangles[RectangleCount - 1].Left = GapLeft + GAP_WIDTH;
				Rectangles[RectangleCount - 1].Right = FIELD_WIDTH;
			}
			// Update the speed at which the player is going.
			PlayerSpeedX += ((float) PlayerAccelX / 32767.0f) * ACCELERATION / 1000;

			// Update the player's position and speed.

			// Left and right edges (X). If the horizontal speed would run the
			// ball into an edge, use up some of the energy in the impact and
			// rebound the ball.
			if (PlayerSpeedX < 0 && PlayerX - PLAYER_SIZE / 2 + PlayerSpeedX / 1000 < 0)
			{
				PlayerX = PLAYER_SIZE / 2
					+ ((PlayerX - PLAYER_SIZE / 2) - (PlayerSpeedX / 1000)) * FIELD_REBOUND;
				PlayerSpeedX = -PlayerSpeedX * FIELD_REBOUND;
			}
			else if (PlayerSpeedX > 0 && PlayerX + PLAYER_SIZE / 2 + PlayerSpeedX / 1000 > FIELD_WIDTH)
			{
				PlayerX = FIELD_WIDTH - PLAYER_SIZE / 2
					+ (FIELD_WIDTH - (PlayerX + PLAYER_SIZE / 2) - (PlayerSpeedX / 1000)) * FIELD_REBOUND;
				PlayerSpeedX = -PlayerSpeedX * FIELD_REBOUND;
			}
			else
			{
				PlayerX += PlayerSpeedX / 1000;
			}

			// Is the ball on a rectangle?
			bool OnRectangle = false;

			for (i = RectangleCount - 1; i >= 0; i--)
			{
				// Stop considering rectangles if they are higher than the ball
				// (Y).
				if (PlayerY - PLAYER_SIZE / 2 < Rectangles[i].Top + LOWER_EPSILON)
					break;
				// If the ball is in range (X)...
				// TODO Circle physics.
				if ((PlayerX - PLAYER_SIZE / 2 >= Rectangles[i].Left
				  && PlayerX - PLAYER_SIZE / 2 <= Rectangles[i].Right)
				 || (PlayerX + PLAYER_SIZE / 2 >= Rectangles[i].Left
				  && PlayerX + PLAYER_SIZE / 2 <= Rectangles[i].Right))
				{
					// Is the distance in range, and is the ball going up slowly
					// enough (Y)?
					if (PlayerY - PLAYER_SIZE / 2 <  Rectangles[i].Top + UPPER_EPSILON
					 && PlayerSpeedY              >= 0.0f
					 && PlayerSpeedY              <  UPPER_EPSILON)
					{
						OnRectangle = true;
						PlayerSpeedY = 0.0f;
						// Also make sure the ball appears to be on the rectangle (Y).
						PlayerY = Rectangles[i].Top + PLAYER_SIZE / 2;
						break;
					}
					// If the ball would cross the rectangle during this
					// millisecond, make it rebound instead (Y).
					else if (PlayerY - PLAYER_SIZE / 2                       >= Rectangles[i].Top
					      && PlayerY - PLAYER_SIZE / 2 + PlayerSpeedY / 1000 <  Rectangles[i].Top)
					{
						PlayerY = PlayerY
							+ ((PlayerY - PLAYER_SIZE / 2) - Rectangles[i].Top - (PlayerSpeedY / 1000)) * RECT_REBOUND;
						PlayerSpeedY = -PlayerSpeedY * RECT_REBOUND;
						break;
					}
				}
			}

			if (OnRectangle)
			{
				// If so, apply friction to the player's speed (X).
				PlayerSpeedX *= 1.0f - FRICTION;
			}
			else
			{
				// If not, apply gravity (Y).
				PlayerSpeedY += GRAVITY / 1000;
				PlayerY += PlayerSpeedY / 1000;
			}

			// Bottom edge (Y).
			// If the ball arrives below the bottom edge, act as if it had
			// arrived on the bottom edge.
			if (PlayerY - PLAYER_SIZE / 2 < 0)
			{
				PlayerY = PLAYER_SIZE / 2;
				PlayerSpeedY = -PlayerSpeedY * FIELD_REBOUND;
			}

			// If the ball has collided with the top of the field,
			// and it is not going down, the player's game is over.
			if (PlayerY + PLAYER_SIZE / 2 >= FIELD_HEIGHT
			 && PlayerSpeedY              >= 0.0f)
			{
				ToScore(Score);
				break;
			}
		}

		AdvanceBackground(Milliseconds);
	}

	AnimationControl(Milliseconds);
}

void GameOutputFrame()
{
	// Draw the background.
	DrawBackground();

	// Draw the rectangles.
	uint32_t i;
	for (i = 0; i < RectangleCount; i++)
	{
		SDL_Rect ColumnDestRect = {
			.x = (int) roundf(Rectangles[i].Left * SCREEN_WIDTH / FIELD_WIDTH),
			.y = SCREEN_HEIGHT - (int) roundf(Rectangles[i].Top * SCREEN_HEIGHT / FIELD_HEIGHT),
			.w = (int) ((Rectangles[i].Right - Rectangles[i].Left) * SCREEN_WIDTH / FIELD_WIDTH),
			.h = (int) ((Rectangles[i].Top - Rectangles[i].Bottom) * SCREEN_HEIGHT / FIELD_HEIGHT)
		};
		SDL_FillRect(Screen, &ColumnDestRect, SDL_MapRGB(Screen->format, 128, 128, 128));
	}

	// Draw the character.
	DRAW_FillCircle(Screen, (int) roundf(PlayerX * SCREEN_WIDTH / FIELD_WIDTH), (int) roundf(SCREEN_HEIGHT - PlayerY * SCREEN_HEIGHT / FIELD_HEIGHT), (int) ((PLAYER_SIZE / 2) * SCREEN_WIDTH / FIELD_WIDTH), SDL_MapRGB(Screen->format, 255, 255, 255));

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
	PlayerX = FIELD_WIDTH / 2;
	PlayerY = FIELD_HEIGHT - PLAYER_SIZE / 2;
	PlayerSpeedX = 0.0f;
	PlayerSpeedY = GRAVITY / 200 - FIELD_SCROLL / 200;
	PlayerAccelX = 0;
	if (Rectangles != NULL)
	{
		free(Rectangles);
		Rectangles = NULL;
	}
	RectangleCount = 0;
	GenDistance = RECT_GEN_START;

	GatherInput = GameGatherInput;
	DoLogic     = GameDoLogic;
	OutputFrame = GameOutputFrame;
}
