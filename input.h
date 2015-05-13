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
#pragma once

#include <stdbool.h>

#include <SDL.h>


#ifdef __GCW0__
// Number determining which "joystick" to use
// -1 means don't use joystick
// 0 is the analog nub
// 1 is G sensor
extern int JoystickIndex;
#endif

void InputInit(void);
void InputFree(void);

void InputOnEvent(const SDL_Event* event);

// GetMovement returns, after updating the platform-specific input status, a
//   value between -32768 and +32767 to indicate how far the ball needs to go.
//   Negative values go to the left; positive values go to the right.
int16_t GetMovement(const int player);
void ResetMovement(void);

void InputSwitchJoystick(const int inc);