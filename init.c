/*
 * Ativayeban, initialisation code file
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
#include <stddef.h>

#include "SDL.h"
#include "SDL_image.h"

#include "gap.h"
#include "main.h"
#include "init.h"
#include "platform.h"
#include "player.h"
#include "title.h"

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

	PlayerSpritesheet = IMG_Load("penguin_ball.png");
	if (PlayerSpritesheet == NULL)
	{
		*Continue = false;  *Error = true;
		printf("IMG_Load failed: %s\n", SDL_GetError());
		SDL_ClearError();
		return;
	}

	if (!GapSurfacesLoad(&GapSurfaces))
	{
		*Continue = false;  *Error = true;
		printf("IMG_Load failed: %s\n", SDL_GetError());
		SDL_ClearError();
		return;
	}

#define LOAD_SOUND(_sound, _path)\
	_sound = Mix_LoadWAV(_path);\
	if (_sound == NULL)\
	{\
		*Continue = false;  *Error = true;\
		printf("Mix_LoadWAV failed: %s\n", SDL_GetError());\
		SDL_ClearError();\
		return;\
	}
	LOAD_SOUND(SoundPlayerBounce, "bounce.ogg");
	LOAD_SOUND(SoundPlayerRoll, "roll.ogg");

	SDL_ShowCursor(0);

	InitializePlatform();

	// Title screen. (-> title.c)
	ToTitleScreen();
}

void Finalize()
{
	SDL_FreeSurface(PlayerSpritesheet);
	GapSurfacesFree(&GapSurfaces);
	Mix_FreeChunk(SoundPlayerBounce);
	Mix_FreeChunk(SoundPlayerRoll);
	SDL_Quit();
}
