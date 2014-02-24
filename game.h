/*
 * Ativayeban, game header
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

#ifndef _GAME_H_
#define _GAME_H_

// When determining whether the ball is on a rectangle, the distance between
// the bottom of the ball and the top of the rectangle can be less than this
// and the ball will be considered to be on the rectangle. This avoids rebound
// from applying indefinitely.
// Given in meters.
#define UPPER_EPSILON    0.02f

// When determining whether the ball is on a rectangle, the distance between
// the bottom of the ball and the top of the rectangle must be more than this.
// This is meant to guard against fuzzy floating-point equality comparisons.
// Given in meters.
#define LOWER_EPSILON   -0.001f

// All speed and acceleration modifiers follow the same directions.
// Vertically: Positive values go upward, and negative values go downward.
// Horizontally: Positive values go rightward, and negative values go leftward.

// The horizontal acceleration when the user presses to go to the right fully.
// Negated and multiplied by a fraction when going to the left or not fully in
// one direction, respectively.
// Given in (meters per second) per second (m/s^2).
#define ACCELERATION     4.00f

// The gravitational force exerted by the bottom of the screen.
// Given in (meters per second) per second (m/s^2).
#define GRAVITY         -9.78f /* like Earth */

// The friction exerted on the ball by the rectangles. It places an implicit
// maximum on the speed allowable for the ball, as well as stops the ball
// slowly when the player stops accelerating in one direction.
// Given as the proportion of the original speed lost per millisecond.
#define FRICTION         0.002f

// The proportion of the ball's energy that remains in it after hitting a
// rectangle. The ball goes against its collision trajectory after rebounding.
#define RECT_REBOUND     0.20f

// The proportion of the ball's energy that remains in it after hitting one of
// the edges of the field. The ball goes against its collision trajectory,
// but only horizontally, after rebounding.
#define FIELD_REBOUND    0.50f

// The speed at which the screen scrolls.
// Given in meters per second (m/s).
#define FIELD_SCROLL     1.00f

// The distance between the edges of two successive rectangles to begin with.
// Given in meters.
#define RECT_GEN_START   2.00f

// The change in distance between the edges of two successive rectangles as
// the player passes through each of them.
// Given in meters (per rectangle).
#define RECT_GEN_SPEED  -0.01f

// The height of each rectangle.
// Given in meters.
#define RECT_HEIGHT      0.25f

// The width of the area to leave empty for the player to pass through.
// Given in meters.
#define GAP_WIDTH        0.75f

// The width and height of the player's character.
// Given in meters.
#define PLAYER_SIZE      0.37f

// The width of the playing field.
// Given in meters.
#define FIELD_WIDTH      5.33f

#define FIELD_HEIGHT     (SCREEN_HEIGHT * (FIELD_WIDTH / SCREEN_WIDTH))

struct AtivayebanRect
{
	float Left;
	float Top;
	float Right;
	float Bottom;
	bool  Passed;
};

extern void ToGame(void);

#endif /* !defined(_GAME_H_) */
