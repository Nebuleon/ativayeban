/*
 * Ativayeban, score screen code file
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

#include <stdbool.h>
#include <stdint.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include "SDL.h"

#include "main.h"
#include "init.h"
#include "platform.h"
#include "sound.h"
#include "text.h"
#include "game.h"
#include "score.h"
#include "bg.h"
#include "sys_specifics.h"

static bool  WaitingForRelease = false;

static char ScoreMessage[256];

SDL_Surface *GameOverImages[2];
static int gameOverImageIndex = 0;
#define GAME_OVER_IMAGE_COUNTER 20
static int gameOverImageCounter = GAME_OVER_IMAGE_COUNTER;

void ScoreGatherInput(bool* Continue)
{
	SDL_Event ev;

	while (SDL_PollEvent(&ev))
	{
		if (IsEnterGamePressingEvent(&ev))
			WaitingForRelease = true;
		else if (IsEnterGameReleasingEvent(&ev))
		{
			WaitingForRelease = false;
			ToGame();
			return;
		}
		else if (IsExitGameEvent(&ev))
		{
			*Continue = false;
			return;
		}
	}
}

void ScoreDoLogic(bool* Continue, bool* Error, Uint32 Milliseconds)
{
	(void)Continue;
	(void)Error;
	(void)Milliseconds;
}

static void DrawGameoverImg(const int i);
void ScoreOutputFrame(void)
{
	DrawBackground(&BG);
	if (SDL_MUSTLOCK(Screen))
		SDL_LockSurface(Screen);
	gameOverImageCounter--;
	if (gameOverImageCounter == 0)
	{
		gameOverImageCounter = GAME_OVER_IMAGE_COUNTER;
		gameOverImageIndex = 1 - gameOverImageIndex;
	}
	DrawGameoverImg(gameOverImageIndex);
	TextRenderCentered(Screen, font, ScoreMessage, SCREEN_HEIGHT / 2 - 10);
	if (SDL_MUSTLOCK(Screen))
		SDL_UnlockSurface(Screen);

	SDL_Flip(Screen);
}
static void DrawGameoverImg(const int i)
{
	SDL_Rect dest = {
		(Sint16)((SCREEN_WIDTH - GameOverImages[i]->w) / 2),
		(Sint16)((SCREEN_HEIGHT - GameOverImages[i]->h) / 2 - SCREEN_HEIGHT / 6),
		0,
		0
	};
	SDL_BlitSurface(GameOverImages[i], NULL, Screen, &dest);
}

void ToScore(uint32_t Score)
{
	SoundStopRoll();
	ResetMovement();
	WaitingForRelease = false;
	SoundPlay(SoundLose, 1.0);
	sprintf(
		ScoreMessage,
		"Your score was %" PRIu32 "\n\nPress %s to play again\nor %s to exit",
		Score, GetEnterGamePrompt(), GetExitGamePrompt());

	GatherInput = ScoreGatherInput;
	DoLogic     = ScoreDoLogic;
	OutputFrame = ScoreOutputFrame;
}
