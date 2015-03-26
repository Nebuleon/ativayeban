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

#include "main.h"
#include "init.h"
#include "platform.h"
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


void TitleScreenGatherInput(bool* Continue)
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

void TitleScreenDoLogic(bool* Continue, bool* Error, Uint32 Milliseconds)
{
	(void)Continue;
	(void)Error;
	(void)Milliseconds;
}

static void DrawTitleImg(const int i);
void TitleScreenOutputFrame(void)
{
	DrawBackground(&BG);

	if (SDL_MUSTLOCK(Screen))
		SDL_LockSurface(Screen);
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
	TextRenderCentered(Screen, font, WelcomeMessage, SCREEN_HEIGHT / 2 - 30);
	if (SDL_MUSTLOCK(Screen))
		SDL_UnlockSurface(Screen);

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
	sprintf(
		WelcomeMessage,
		"Press %s to play\nor %s to exit\n\nIn-game:\n%s to move around\n%s to pause\n%s to exit",
		GetEnterGamePrompt(), GetExitGamePrompt(), GetMovementPrompt(), GetPausePrompt(), GetExitGamePrompt());

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
	return true;
}
void TitleImagesFree(void)
{
	for (int i = 0; i < 12; i++)
	{
		SDL_FreeSurface(TitleImages[i]);
	}
}
