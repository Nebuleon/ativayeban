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

#include "player.h"
#include "utils.h"


#define P1_LEFT SDL_SCANCODE_LEFT
#define P1_RIGHT SDL_SCANCODE_RIGHT
#define P1_CALL1 SDL_SCANCODE_UP
#define P1_CALL2 SDL_SCANCODE_DOWN
#ifdef __GCW0__
#define P2_LEFT0 SDL_SCANCODE_LSHIFT
#define P2_RIGHT0 SDL_SCANCODE_LCTRL
#define P2_LEFT1 SDL_SCANCODE_SPACE
#define P2_RIGHT1 SDL_SCANCODE_LALT
#else
#define P2_LEFT0 SDL_SCANCODE_A
#define P2_RIGHT0 SDL_SCANCODE_D
#define P2_LEFT1 SDL_SCANCODE_Z
#define P2_RIGHT1 SDL_SCANCODE_X
#define P2_CALL1 SDL_SCANCODE_W
#define P2_CALL2 SDL_SCANCODE_S
#endif

static bool pressed[SDL_NUM_SCANCODES];
#define JOY_DEADZONE 2000
#ifdef __GCW0__
static SDL_Joystick *analog = NULL;
static SDL_Joystick *gSensor = NULL;
static int16_t gZero = 0;
int JoystickIndex = -1;
#define G_SENSITIVITY 7
#else
static SDL_Joystick *joysticks[MAX_PLAYERS];
#endif


void InputInit(void)
{
	memset(pressed, 0, sizeof pressed);
	for (int i = 0; i < SDL_NumJoysticks(); i++)
	{
#ifdef __GCW0__
		// Look for the right joysticks by name
		if (strcmp(SDL_JoystickName(i), "linkdev device (Analog 2-axis 8-button 2-hat)") == 0)
			analog = SDL_JoystickOpen(i);
		else if (strcmp(SDL_JoystickName(i), "mxc6225") == 0)
			gSensor = SDL_JoystickOpen(i);
#else
		joysticks[i] = SDL_JoystickOpen(i);
#endif
	}
}
void InputFree(void)
{
#ifdef __GCW0__
	if (analog) SDL_JoystickClose(analog);
	if (gSensor) SDL_JoystickClose(gSensor);
#endif
}

void InputOnEvent(const SDL_Event* event)
{
	switch (event->type)
	{
	case SDL_KEYUP:
	case SDL_KEYDOWN:
		pressed[event->key.keysym.scancode] = event->type == SDL_KEYDOWN;
		break;
	default:
		break;
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
			const int16_t x = GetJoyX(analog, 0);
			if (abs(x) > JOY_DEADZONE) return x;
		}
		else if (JoystickIndex == 1)
		{
			// G sensor
			const int gx = GetJoyX(gSensor, gZero) * G_SENSITIVITY;
			return (int16_t)CLAMP(gx, -32768, 32767);
		}
	}
	const bool left = player == 0 ?
		pressed[P1_LEFT] : (pressed[P2_LEFT0] || pressed[P2_LEFT1]);
	const bool right = player == 0 ?
		pressed[P1_RIGHT] : (pressed[P2_RIGHT0] || pressed[P2_RIGHT1]);
	return left ? (right ? 0 : -32768) : (right ? 32767 : 0);
#else
	if (player < NumJoysticks)
	{
		// Joystick player; return analog stick
		const int16_t x = GetJoyX(joysticks[player], 0);
		if (abs(x) > JOY_DEADZONE) return x;
		return 0;
		// TODO: use D-pad as well
	}
	else
	{
		// Keyboards
		const bool left = player == NumJoysticks ?
			pressed[P1_LEFT] : (pressed[P2_LEFT0] || pressed[P2_LEFT1]);
		const bool right = player == NumJoysticks ?
			pressed[P1_RIGHT] : (pressed[P2_RIGHT0] || pressed[P2_RIGHT1]);
		return left ? (right ? 0 : -32768) : (right ? 32767 : 0);
	}
#endif
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
	if (JoystickIndex == 1 && gSensor)
	{
		// Recalibrate G sensor
		gZero = (int16_t)SDL_JoystickGetAxis(gSensor, 0);
	}
#else
	UNUSED(inc);
#endif
}

bool InputIsCalling(const int player)
{
#ifdef __GCW0__
	if (player == 0)
	{
		// Left shoulder
		return pressed[SDLK_TAB];
	}
	else
	{
		// Right shoulder
		return pressed[SDLK_BACKSPACE];
	}
#else
	if (player < NumJoysticks)
	{
		// Joystick player; return button 1
		return SDL_JoystickGetButton(joysticks[player], 0);
	}
	else
	{
		// Keyboards
		return player == NumJoysticks ?
			(pressed[P1_CALL1] || pressed[P1_CALL2]) :
			(pressed[P2_CALL1] || pressed[P2_CALL2]);
	}
#endif
}
