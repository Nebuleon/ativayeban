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

#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>

#include "SDL_image.h"

#include "animation.h"
#include "box.h"
#include "main.h"
#include "high_score.h"
#include "init.h"
#include "input.h"
#include "particle.h"
#include "platform.h"
#include "player.h"
#include "sound.h"
#include "space.h"
#include "text.h"
#include "game.h"
#include "bg.h"
#include "sys_specifics.h"

static bool Start = false;
static char WelcomeMessage[256];
static int winnerIndices[MAX_PLAYERS];
static int winners = 0;
static Animation TitleAnim;
static Animation GameOverAnim;
static HighScoreDisplay HSD;
Tex ControlTexes[MAX_PLAYERS];
#ifdef __GCW0__
Tex ControlTex0Analog = NULL;
Tex ControlTex0G = NULL;
#endif

static Block blocks[MAX_PLAYERS];
#define BLOCK_WIDTH (FIELD_WIDTH / MAX_PLAYERS * 0.25f)
#define BLOCK_Y (FIELD_HEIGHT * 0.5f)

// Countdown to start the game automatically; starts when a player is enabled
static int countdownMs = -1;
#define COUNTDOWN_START_MS 3999

bool playersEnabled[MAX_PLAYERS];


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
#ifdef __GCW0__
		// Enable/disable G-Sensor based on up/down
		if (ev.type == SDL_KEYUP &&
			(ev.key.keysym.sym == SDLK_UP || ev.key.keysym.sym == SDLK_DOWN))
		{
			InputSwitchJoystick(ev.key.keysym.sym == SDLK_UP ? -1 : 1);
			SoundPlay(SoundScore, 1.0);
		}
#endif
	}
}
static void TitleScreenEnd(void)
{
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		BlockRemove(&blocks[i]);
		// Kill players that have not been enabled
		players[i].Enabled = playersEnabled[i];
		players[i].Alive = playersEnabled[i];
	}
}

void TitleScreenDoLogic(bool* Continue, bool* Error, Uint32 Milliseconds)
{
	(void)Continue;
	(void)Error;
	cpSpaceStep(space.Space, Milliseconds * 0.001);
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		PlayerUpdate(&players[i], Milliseconds);

		// Check which players have fallen below their start pads
		cpVect pos = cpBodyGetPosition(players[i].Body);
		if (pos.y < BLOCK_Y)
		{
			if (!playersEnabled[i])
			{
				// New player entered
				countdownMs = COUNTDOWN_START_MS;
				SoundPlay(SoundStart, 1.0);
			}
			playersEnabled[i] = true;
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

	Animation *a = Start ? &TitleAnim : &GameOverAnim;
	AnimationUpdate(a, Milliseconds);

	HighScoreDisplayUpdate(&HSD, Milliseconds);
}

static Tex GetControlTex(const int i);
static void DrawTitleImg(void);
void TitleScreenOutputFrame(void)
{
	DrawBackground(&BG, 0);

	HighScoreDisplayDraw(&HSD);

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		Tex t = GetControlTex(i);
		SDL_Rect dest =
		{
			SCREEN_X((i + 1) * FIELD_WIDTH / (MAX_PLAYERS + 1)) -
				t.W / 2,
			(SCREEN_HEIGHT - t.H) / 2 - SCREEN_X(PLAYER_RADIUS),
			t.W, t.H
		};
		RenderTex(t.T, NULL, &dest);
	}

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		PlayerDraw(&players[i], 0);
	}

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		BlockDraw(&blocks[i], 0);
	}

	DrawTitleImg();
	// Draw player icons if winners
	if (!Start)
	{
		const int left =
			(SCREEN_WIDTH - PLAYER_SPRITESHEET_WIDTH * winners) / 2;
		for (int i = 0; i < winners; i++)
		{
			const int playerIndex = winnerIndices[i];
			SDL_Rect src = {
				0, 0, PLAYER_SPRITESHEET_WIDTH, PLAYER_SPRITESHEET_HEIGHT
			};
			SDL_Rect dest =
			{
				left + i * PLAYER_SPRITESHEET_WIDTH,
				(int)(SCREEN_HEIGHT * 0.66f),
				src.w, src.h
			};
			RenderTex(PlayerSpritesheets[playerIndex].T, &src, &dest);
		}
	}
	SDL_Color c = { 177, 177, 177, 255 };
	TextRenderCentered(
		font, WelcomeMessage, (int)(SCREEN_HEIGHT * 0.75f), c);

	SDL_RenderPresent(Renderer);
}
static Tex GetControlTex(const int i)
{
	Tex t = ControlTexes[i];
#ifdef __GCW0__
	if (i == 0)
	{
		switch (JoystickIndex)
		{
		case 0:
			t = ControlTex0Analog;
			break;
		case 1:
			t = ControlTex0G;
			break;
		default:
			// Do nothing
			break;
		}
	}
#endif
	return t;
}
static void DrawTitleImg(void)
{
	const Animation *a = Start ? &TitleAnim : &GameOverAnim;
	AnimationDrawUpperCenter(a);
}

void ToTitleScreen(const bool start)
{
	countdownMs = -1;
	ResetMovement();
	MusicSetLoud(false);
	BackgroundsInit(&BG);
	Start = start;
	if (Start)
	{
		sprintf(
			WelcomeMessage,
			"%s to pause\n%s to exit",
			GetPausePrompt(), GetExitGamePrompt());
	}
	else
	{
		// Find out the result of the game
		int maxScore = 0;
		for (int i = 0; i < MAX_PLAYERS; i++)
		{
			if (players[i].Score > maxScore) maxScore = players[i].Score;
		}
		winners = 0;
		for (int i = 0; i < MAX_PLAYERS; i++)
		{
			if (!players[i].Enabled) continue;
			if (players[i].Score == maxScore)
			{
				winnerIndices[winners] = i;
				winners++;
			}
		}
		if (PlayerEnabledCount() == 1)
		{
			sprintf(
				WelcomeMessage,
				"Your score was %d!\n%s to exit",
				maxScore, GetExitGamePrompt());
		}
		else if (winners == 1)
		{
			sprintf(
				WelcomeMessage,
				"Wins with score %d!\n%s to exit",
				maxScore, GetExitGamePrompt());
		}
		else
		{
			sprintf(
				WelcomeMessage,
				"Tied with score %d!\n%s to exit",
				maxScore, GetExitGamePrompt());
		}
		HighScoresAdd(maxScore);
	}

	HighScoreDisplayInit(&HSD);

	CArrayClear(&Particles);
	SpaceReset(&space);
	// Add bottom edge so we don't fall through
	SpaceAddBottomEdge(&space);

	// Initialise players here
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		PlayerInit(&players[i], i, cpv(
			(i + 1) * FIELD_WIDTH / (MAX_PLAYERS + 1),
			FIELD_HEIGHT * 0.75f));
		playersEnabled[i] = false;
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
	if (!AnimationLoad(&TitleAnim, "data/graphics/anim.png", 169, 40, 12))
	{
		return false;
	}
	if (!AnimationLoad(
		&GameOverAnim, "data/graphics/gameover.png", 131, 40, 3))
	{
		return false;
	}
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		char buf[256];
#ifdef __GCW0__
		sprintf(buf, "data/graphics/gcw%d.png", i);
#else
		sprintf(buf, "data/graphics/keyboard%d.png", i);
#endif
		ControlTexes[i] = LoadTex(buf);
		if (ControlTexes[i].T == NULL)
		{
			return false;
		}
	}
#ifdef __GCW0__
	ControlTex0Analog = LoadTex("data/graphics/gcw0analog.png");
	if (ControlTex0Analog.T == NULL) return false;
	ControlTex0G = LoadTex("data/graphics/gcw0g.png");
	if (ControlTex0G.T == NULL) return false;
#endif
	return true;
}
void TitleImagesFree(void)
{
	AnimationFree(&TitleAnim);
	AnimationFree(&GameOverAnim);
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		SDL_DestroyTexture(ControlTexes[i].T);
	}
#ifdef __GCW0__
	SDL_DestroyTexture(ControlTex0Analog.T);
	SDL_DestroyTexture(ControlTex0G.T);
#endif
}
