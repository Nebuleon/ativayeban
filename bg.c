/*
 * Ativayeban, background rendering code file
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

#include <math.h>

#include "SDL.h"

#include "main.h"

void AdvanceBackground(uint32_t Milliseconds)
{
}

void DrawBackground(void)
{
	SDL_Rect ScreenRect = { .x = 0, .y = 0, .w = Screen->w, .h = Screen->h };
	SDL_FillRect(Screen, &ScreenRect, SDL_MapRGB(Screen->format, 0, 0, 0));
}
