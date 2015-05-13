/*
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
#include "input.h"

#include <stdbool.h>

#include "utils.h"


#define P1_LEFT SDLK_LEFT
#define P1_RIGHT SDLK_RIGHT
#ifdef __GCW0__
#define P2_LEFT0 SDLK_LSHIFT
#define P2_RIGHT0 SDLK_LCTRL
#define P2_LEFT1 SDLK_SPACE
#define P2_RIGHT1 SDLK_LALT
#else
#define P2_LEFT0 SDLK_a
#define P2_RIGHT0 SDLK_d
#define P2_LEFT1 SDLK_z
#define P2_RIGHT1 SDLK_x
#endif

static bool pressed[SDLK_LAST];
// TODO: support joysticks for PC
#ifdef __GCW0__
static SDL_Joystick *joysticks[2];
static int16_t gZero = 0;
int JoystickIndex = -1;
#define JOY_DEADZONE 500
#define G_SENSITIVITY 5
#endif


void InputInit(void)
{
	memset(pressed, 0, sizeof pressed);
#ifdef __GCW0__
	memset(joysticks, 0, sizeof joysticks);
	for (int i = 0; i < 2 && i < SDL_NumJoysticks(); i++)
	{
		joysticks[i] = SDL_JoystickOpen(i);
	}
#endif
}
void InputFree(void)
{
#ifdef __GCW0__
	for (int i = 0; i < 2; i++)
	{
		if (joysticks[i] != NULL && SDL_JoystickOpened(i))
		{
			SDL_JoystickClose(joysticks[i]);
		}
	}
#endif
}

void InputOnEvent(const SDL_Event* event)
{
	if (event->type == SDL_KEYUP || event->type == SDL_KEYDOWN)
	{
		pressed[event->key.keysym.sym] = event->type == SDL_KEYDOWN;
	}
}

static int16_t GetJoyX(SDL_Joystick *joy, const int16_t zero);
int16_t GetMovement(const int player)
{
#ifdef __GCW0__
	if (player == 0)
	{
		if (JoystickIndex == 0)
		{
			// Analog nub
			const int16_t x = GetJoyX(joysticks[0], 0);
			if (abs(x) > JOY_DEADZONE) return x;
		}
		else if (JoystickIndex == 1)
		{
			// G sensor
			const int gx = GetJoyX(joysticks[1], joystickZero) * G_SENSITIVITY;
			return (int16_t)CLAMP(gx, -32768, 32767);
		}
	}
#endif
	const bool left = player == 0 ?
		pressed[P1_LEFT] : (pressed[P2_LEFT0] || pressed[P2_LEFT1]);
	const bool right = player == 0 ?
		pressed[P1_RIGHT] : (pressed[P2_RIGHT0] || pressed[P2_RIGHT1]);
	return left ? (right ? 0 : -32768) : (right ? 32767 : 0);
}
static int16_t GetJoyX(SDL_Joystick *joy, const int16_t zero)
{
	if (!joy) return 0;
	// Read X-axis of chosen joystick
	return (int16_t)SDL_JoystickGetAxis(joy, 0) + zero;
}

void ResetMovement(void)
{
	memset(pressed, 0, sizeof pressed);
}

void InputSwitchJoystick(const int inc)
{
#ifdef __GCW0__
	JoystickIndex = CLAMP_OPPOSITE(JoystickIndex + inc, -1, 1);
	if (JoystickIndex == 1 && joysticks[1])
	{
		// Recalibrate G sensor
		gZero = (int16_t)SDL_JoystickGetAxis(joysticks[1], 0);
	}
#else
	UNUSED(inc);
#endif
}
