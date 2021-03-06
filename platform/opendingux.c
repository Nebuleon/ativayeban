/*
 * Ativayeban, OpenDingux platform-specific code file
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

static bool    LeftPressed;
static bool    RightPressed;

void InitializePlatform(void)
{
}

Uint32 ToNextFrame(void)
{
	// OpenDingux waits for vertical sync by itself.
	return 16;
}

bool IsEnterGamePressingEvent(const SDL_Event* event)
{
	return event->type == SDL_KEYDOWN
	    && (event->key.keysym.sym == SDLK_LCTRL  /* A */
	     || event->key.keysym.sym == SDLK_RETURN /* Start */);
}

bool IsEnterGameReleasingEvent(const SDL_Event* event)
{
	return event->type == SDL_KEYUP
	    && (event->key.keysym.sym == SDLK_LCTRL  /* A */
	     || event->key.keysym.sym == SDLK_RETURN /* Start */);
}

const char* GetEnterGamePrompt(void)
{
	return "A/Start";
}

bool IsExitGameEvent(const SDL_Event* event)
{
	return event->type == SDL_QUIT
	    || (event->type == SDL_KEYDOWN
	     && (event->key.keysym.sym == SDLK_LALT   /* B */
	      || event->key.keysym.sym == SDLK_ESCAPE /* Select */));
}

const char* GetExitGamePrompt(void)
{
	return "B/Select";
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
	return "D-pad Left/Right";
}

bool IsPauseEvent(const SDL_Event* event)
{
	return event->type == SDL_KEYDOWN
	    && event->key.keysym.sym == SDLK_RETURN /* Start */;
}

const char* GetPausePrompt(void)
{
	return "Start";
}

void ResetMovement(void)
{
	LeftPressed = RightPressed = false;
}
