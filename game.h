/*
 * Ativayeban, game header
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

#include <math.h>

#include <SDL_mixer.h>
#include <SDL_ttf.h>

#include "init.h"

// All speed and acceleration modifiers follow the same directions.
// Vertically: Positive values go upward, and negative values go downward.
// Horizontally: Positive values go rightward, and negative values go leftward.

// The horizontal acceleration when the user presses to go to the right fully.
// Negated and multiplied by a fraction when going to the left or not fully in
// one direction, respectively.
// Given in (meters per second) per second (m/s^2).
#define ACCELERATION     50.00f
// Acceleration when at max speed
#define ACCELERATION_MAX_SPEED 10.0f
#define MAX_SPEED 6.0f
#define MIN_SPEED 2.0f
// Extra acceleration when below min speed
#define MIN_SPEED_ACCEL_BONUS 100.0f
// Extra acceleration when rolling
#define ROLL_ACCEL_BONUS 50.0f

// The gravitational force exerted by the bottom of the screen.
// Given in (meters per second) per second (m/s^2).
#define GRAVITY         -9.78f /* like Earth */

// The speed at which the screen scrolls.
// Given in meters per second (m/s).
#define FIELD_SCROLL     1.60f
#define FIELD_SCROLL_MAX 1.86f

// The speed at which the scroll speed increases, every second
#define FIELD_SCROLL_SPEED 0.01f

#define FIELD_ELASTICITY 0.25f

// The distance between the edges of two successive gaps to begin with.
// Given in meters.
#define GAP_GEN_START   1.50f

#define GAP_GEN_MIN   1.0f

// The change in distance between the edges of two successive gaps as
// the player passes through each of them.
// Given in meters (per rectangle).
#define GAP_GEN_SPEED  -0.01f

// The width of the area to leave empty for the player to pass through.
// Given in meters.
#define GAP_WIDTH        0.75f

// The height of gap surfaces.
// Given in meters.
#define GAP_HEIGHT       0.25f

#define MAX_GAPS 3

#define MIN_BLOCK_WIDTH 0.25f

#define BLOCK_ELASTICITY 0.25f

// The radius of the player's character.
// Given in meters.
#define PLAYER_RADIUS    0.185f

#define PLAYER_ELASTICITY 1.0f

// The width of the playing field.
// Given in meters.
#define FIELD_WIDTH      5.33f

#define FIELD_HEIGHT     (SCREEN_HEIGHT * (FIELD_WIDTH / SCREEN_WIDTH))

// Convert game coordinates to screen coordinates
#define SCREEN_X(_x) ((int)roundf((_x) * SCREEN_WIDTH / FIELD_WIDTH))
#define SCREEN_Y(_y) ((int)roundf(SCREEN_HEIGHT - (_y) * SCREEN_HEIGHT / FIELD_HEIGHT))

extern Mix_Chunk* SoundBeep;
extern Mix_Chunk* SoundStart;
extern Mix_Chunk* SoundLose;
extern Mix_Chunk* SoundScore;

extern TTF_Font *font;

extern void ToGame(void);
