/*
 * Ativayeban, initialisation code file
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
#include <stddef.h>

#include "SDL.h"
#include "SDL_image.h"

#include "gap.h"
#include "game.h"
#include "main.h"
#include "init.h"
#include "platform.h"
#include "player.h"
#include "space.h"
#include "sound.h"
#include "title.h"

SDL_Surface *icon = NULL;

void Initialize(bool* Continue, bool* Error)
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK) < 0)
	{
		*Continue = false;  *Error = true;
		printf("SDL initialisation failed: %s\n", SDL_GetError());
		SDL_ClearError();
		return;
	}
	else
		printf("SDL initialisation succeeded\n");

	Screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32, SDL_HWSURFACE |
#ifdef SDL_TRIPLEBUF
		SDL_TRIPLEBUF
#else
		SDL_DOUBLEBUF
#endif
		);

	if (Screen == NULL)
	{
		*Continue = false;  *Error = true;
		printf("SDL_SetVideoMode failed: %s\n", SDL_GetError());
		SDL_ClearError();
		return;
	}
	else
		printf("SDL_SetVideoMode succeeded\n");

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1)
	{
		*Continue = false;  *Error = true;
		printf("Mix_OpenAudio failed: %s\n", SDL_GetError());
		SDL_ClearError();
		return;
	}
	else
		printf("Mix_OpenAudio succeeded\n");

	if (TTF_Init() == -1)
	{
		*Continue = false;  *Error = true;
		printf("TTF_Init failed: %s\n", SDL_GetError());
		SDL_ClearError();
		return;
	}
	else
		printf("TTF_Init succeeded\n");

	SDL_WM_SetCaption("FallingTime", NULL);

#define LOAD_IMG(_surface, _path)\
	_surface = IMG_Load("data/graphics/" _path);\
	if (_surface == NULL)\
	{\
		*Continue = false;  *Error = true;\
		printf("IMG_Load failed: %s\n", SDL_GetError());\
		SDL_ClearError();\
		return;\
	}
	LOAD_IMG(icon, "icon.png");
	SDL_WM_SetIcon(icon, NULL);

	LOAD_IMG(PlayerSpritesheets[0], "penguin_ball.png");
	LOAD_IMG(PlayerSpritesheets[1], "penguin_black.png");

	if (!GapSurfacesLoad())
	{
		*Continue = false;  *Error = true;
		printf("IMG_Load failed: %s\n", SDL_GetError());
		SDL_ClearError();
		return;
	}
	if (!TitleImagesLoad())
	{
		*Continue = false;  *Error = true;
		printf("IMG_Load failed: %s\n", SDL_GetError());
		SDL_ClearError();
		return;
	}
	if (!BackgroundsLoad(&BG))
	{
		*Continue = false;  *Error = true;
		printf("IMG_Load failed: %s\n", SDL_GetError());
		SDL_ClearError();
		return;
	}

#define LOAD_SOUND(_sound, _path)\
	_sound = Mix_LoadWAV("data/sounds/" _path);\
	if (_sound == NULL)\
	{\
		*Continue = false;  *Error = true;\
		printf("Mix_LoadWAV failed: %s\n", SDL_GetError());\
		SDL_ClearError();\
		return;\
	}
	LOAD_SOUND(SoundBeep, "beep.ogg");
	LOAD_SOUND(SoundPlayerBounce, "bounce.ogg");
	LOAD_SOUND(SoundStart, "start.ogg");
	LOAD_SOUND(SoundLose, "lose.ogg");
	LOAD_SOUND(SoundScore, "score.ogg");
	SoundLoad();

	music = Mix_LoadMUS("data/sounds/music.ogg");
	if (music == NULL)
	{
		*Continue = false;  *Error = true;
		printf("Mix_LoadMUS failed: %s\n", SDL_GetError());
		SDL_ClearError();
		return;
	}

	font = TTF_OpenFont("data/LondrinaSolid-Regular.otf", 20);
	if (font == NULL)
	{
		*Continue = false;  *Error = true;
		printf("TTF_OpenFont failed: %s\n", SDL_GetError());
		SDL_ClearError();
		return;
	}

	SpaceInit(&space);

	SDL_ShowCursor(0);

	InitializePlatform();

	// Title screen. (-> title.c)
	ToTitleScreen(true, 0);
}

void Finalize()
{
	SpaceFree(&space);
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		SDL_FreeSurface(PlayerSpritesheets[i]);
	}
	SDL_FreeSurface(icon);
	GapSurfacesFree();
	TitleImagesFree();
	BackgroundsFree(&BG);
	Mix_FreeChunk(SoundPlayerBounce);
	Mix_FreeChunk(SoundBeep);
	Mix_FreeChunk(SoundStart);
	Mix_FreeChunk(SoundLose);
	Mix_FreeChunk(SoundScore);
	Mix_FreeMusic(music);
	SoundFree();
	TTF_CloseFont(font);
	SDL_Quit();
}
