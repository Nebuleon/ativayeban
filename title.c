/*
 * Ativayeban, title screen code file
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
#include "title.h"

#include <stdlib.h>

#include "SDL_image.h"

#include "block.h"
#include "main.h"
#include "init.h"
#include "input.h"
#include "platform.h"
#include "player.h"
#include "sound.h"
#include "space.h"
#include "text.h"
#include "game.h"
#include "bg.h"
#include "sys_specifics.h"

static bool  WaitingForRelease = false;
static char WelcomeMessage[256];
SDL_Surface *TitleImages[12];
static int titleImageIndex = 0;
#define TITLE_IMAGE_COUNTER 5
static int titleImageCounter = TITLE_IMAGE_COUNTER;
SDL_Surface *ControlSurfaces[MAX_PLAYERS];

static Block blocks[MAX_PLAYERS];
#define BLOCK_WIDTH (FIELD_WIDTH / MAX_PLAYERS * 0.25f)
#define BLOCK_Y (FIELD_HEIGHT * 0.5f)

// Countdown to start the game automatically; starts when a player is enabled
static int countdownMs = -1;
#define COUNTDOWN_START_MS 3999


static void TitleScreenEnd(void);
void TitleScreenGatherInput(bool* Continue)
{
	SDL_Event ev;

	while (SDL_PollEvent(&ev))
	{
		if (IsExitGameEvent(&ev))
		{
			*Continue = false;
			TitleScreenEnd();
			return;
		}
		InputOnEvent(&ev);
		for (int i = 0; i < MAX_PLAYERS; i++)
		{
			players[i].AccelX = GetMovement(i);
		}
	}
}
static void TitleScreenEnd(void)
{
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		BlockRemove(&blocks[i]);
	}
}

void TitleScreenDoLogic(bool* Continue, bool* Error, Uint32 Milliseconds)
{
	(void)Continue;
	(void)Error;
	cpSpaceStep(space.Space, Milliseconds * 0.001);
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		PlayerUpdate(&players[i]);

		// Check which players have fallen below their start pads
		cpVect pos = cpBodyGetPosition(players[i].Body);
		if (pos.y < BLOCK_Y)
		{
			if (!players[i].Enabled)
			{
				// New player entered
				countdownMs = COUNTDOWN_START_MS;
				SoundPlay(SoundStart, 1.0);
			}
			players[i].Enabled = true;
		}
	}

	if (countdownMs >= 0)
	{
		const int countdownMsNext = countdownMs - Milliseconds;
		// Play a beep every second
		if ((countdownMs / 1000) > (countdownMsNext / 1000))
		{
			SoundPlay(SoundBeep, 1.0);
		}
		// Start game if counted down to zero
		if (countdownMsNext <= 0)
		{
			TitleScreenEnd();
			ToGame();
			return;
		}
		countdownMs = countdownMsNext;
	}
}

static void DrawTitleImg(const int i);
void TitleScreenOutputFrame(void)
{
	DrawBackground(&BG, 0);

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		SDL_Rect dest =
		{
			(Sint16)(
				SCREEN_X((i + 1) * FIELD_WIDTH / (MAX_PLAYERS + 1)) -
				ControlSurfaces[i]->w / 2),
			(Sint16)(
				(SCREEN_HEIGHT - ControlSurfaces[i]->h) / 2 -
				SCREEN_X(PLAYER_RADIUS)),
			0,
			0
		};
		SDL_BlitSurface(ControlSurfaces[i], NULL, Screen, &dest);
	}

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		PlayerDraw(&players[i], 0);
	}

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		BlockDraw(&blocks[i], 0);
	}

	titleImageCounter--;
	if (titleImageCounter == 0)
	{
		titleImageCounter = TITLE_IMAGE_COUNTER;
		titleImageIndex++;
		if (titleImageIndex == 12)
		{
			titleImageIndex = 0;
		}
	}
	DrawTitleImg(titleImageIndex);
	TextRenderCentered(Screen, font, WelcomeMessage, SCREEN_HEIGHT * 0.75f);

	SDL_Flip(Screen);
}
static void DrawTitleImg(const int i)
{
	SDL_Rect dest = {
		(Sint16)((SCREEN_WIDTH - TitleImages[i]->w) / 2),
		(Sint16)((SCREEN_HEIGHT - TitleImages[i]->h) / 2 - SCREEN_HEIGHT / 4),
		0,
		0
	};
	SDL_BlitSurface(TitleImages[i], NULL, Screen, &dest);
}

void ToTitleScreen(void)
{
	MusicSetLoud(false);
	Mix_PlayMusic(music, -1);
	BackgroundsInit(&BG);
	sprintf(
		WelcomeMessage,
		"%s to pause\n%s to exit",
		GetPausePrompt(), GetExitGamePrompt());

	// Add bottom edge so we don't fall through
	SpaceAddBottomEdge(&space);

	// Initialise players here
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		PlayerInit(&players[i], i, cpv(
			(i + 1) * FIELD_WIDTH / (MAX_PLAYERS + 1),
			FIELD_HEIGHT * 0.75f));
	}

	// Add platforms for players to jump off
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		BlockInit(
			&blocks[i],
			(i + 1) * FIELD_WIDTH / (MAX_PLAYERS + 1) - BLOCK_WIDTH / 2,
			BLOCK_Y,
			BLOCK_WIDTH);
	}

	GatherInput = TitleScreenGatherInput;
	DoLogic     = TitleScreenDoLogic;
	OutputFrame = TitleScreenOutputFrame;
}

bool TitleImagesLoad(void)
{
	for (int i = 0; i < 12; i++)
	{
		char buf[256];
		sprintf(buf, "data/graphics/anim%02d.png", i + 1);
		TitleImages[i] = IMG_Load(buf);
		if (TitleImages[i] == NULL)
		{
			return false;
		}
	}
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		char buf[256];
#ifdef __GCW0__
		sprintf(buf, "data/graphics/gcw%d.png", i);
#else
		sprintf(buf, "data/graphics/keyboard%d.png", i);
#endif
		ControlSurfaces[i] = IMG_Load(buf);
		if (ControlSurfaces[i] == NULL)
		{
			return false;
		}
	}
	return true;
}
void TitleImagesFree(void)
{
	for (int i = 0; i < 12; i++)
	{
		SDL_FreeSurface(TitleImages[i]);
	}
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		SDL_FreeSurface(ControlSurfaces[i]);
	}
}
