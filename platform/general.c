/*
 * Ativayeban, default platform-specific code file
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
#include <stdint.h>

#include "SDL.h"

#include "platform.h"

static Uint32 LastTicks = 0;
static bool   LeftPressed;
static bool   RightPressed;

void InitializePlatform(void)
{
	LastTicks = SDL_GetTicks();
}

Uint32 ToNextFrame(void)
{
	SDL_Delay(8);
	Uint32 Ticks = SDL_GetTicks();
	Uint32 Duration = Ticks - LastTicks;
	LastTicks = Ticks;
	return Duration;
}

bool IsEnterGamePressingEvent(const SDL_Event* event)
{
	return event->type == SDL_KEYDOWN
	    && (event->key.keysym.sym == SDLK_RETURN
	     || event->key.keysym.sym == SDLK_SPACE);
}

bool IsEnterGameReleasingEvent(const SDL_Event* event)
{
	return event->type == SDL_KEYUP
	    && (event->key.keysym.sym == SDLK_RETURN
	     || event->key.keysym.sym == SDLK_SPACE);
}

const char* GetEnterGamePrompt(void)
{
	return "Enter/Space";
}

bool IsExitGameEvent(const SDL_Event* event)
{
	return event->type == SDL_QUIT
	    || (event->type == SDL_KEYDOWN
	     && event->key.keysym.sym == SDLK_ESCAPE);
}

const char* GetExitGamePrompt(void)
{
	return "Esc";
}

int16_t GetMovement(const SDL_Event* event)
{
	if (event->type == SDL_KEYUP
	 || event->type == SDL_KEYDOWN)
	{
		if (event->key.keysym.sym == SDLK_LEFT)
			LeftPressed = event->type == SDL_KEYDOWN;
		else if (event->key.keysym.sym == SDLK_RIGHT)
			RightPressed = event->type == SDL_KEYDOWN;
	}
	return (LeftPressed)
		? ((RightPressed) ?     0 : -32768)
		: ((RightPressed) ? 32767 :      0);
}

const char* GetMovementPrompt(void)
{
	return "Left/Right";
}

bool IsPauseEvent(const SDL_Event* event)
{
	return event->type == SDL_KEYDOWN
	    && event->key.keysym.sym == SDLK_p;
}

const char* GetPausePrompt(void)
{
	return "P";
}

void ResetMovement(void)
{
	LeftPressed = RightPressed = false;
}
