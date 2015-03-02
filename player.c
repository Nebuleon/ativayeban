#include <math.h>
#include <stdbool.h>
#include <stdint.h>

#include "draw.h"
#include "game.h"
#include "init.h"
#include "main.h"
#include "player.h"

#define PLAYER_SPRITESHEET_WIDTH 35
#define PLAYER_SPRITESHEET_HEIGHT 35
#define PLAYER_SPRITESHEET_STRIDE 8
#define PLAYER_SPRITESHEET_COUNT 16
#define PLAYER_ROLL_SCALE 0.015f

SDL_Surface* PlayerSpritesheet = NULL;

void PlayerUpdate(struct Player *player)
{
	// Update the ball scroll.
	player->Y += FIELD_SCROLL / 1000;

	// Update the speed at which the player is going.
	player->SpeedX += ((float) player->AccelX / 32767.0f) * ACCELERATION / 1000;

	// Update the player's position and speed.

	// Left and right edges (X). If the horizontal speed would run the
	// ball into an edge, use up some of the energy in the impact and
	// rebound the ball.
	if (player->SpeedX < 0 && player->X - PLAYER_SIZE / 2 + player->SpeedX / 1000 < 0)
	{
		player->X = PLAYER_SIZE / 2
			+ ((player->X - PLAYER_SIZE / 2) - (player->SpeedX / 1000)) * FIELD_REBOUND;
		player->SpeedX = -player->SpeedX * FIELD_REBOUND;
	}
	else if (player->SpeedX > 0 && player->X + PLAYER_SIZE / 2 + player->SpeedX / 1000 > FIELD_WIDTH)
	{
		player->X = FIELD_WIDTH - PLAYER_SIZE / 2
			+ (FIELD_WIDTH - (player->X + PLAYER_SIZE / 2) - (player->SpeedX / 1000)) * FIELD_REBOUND;
		player->SpeedX = -player->SpeedX * FIELD_REBOUND;
	}
	else
	{
		player->X += player->SpeedX / 1000;
	}

	// Update roll animation based on speed
	player->Roll += player->SpeedX * PLAYER_ROLL_SCALE;
	if (player->Roll < 0)
	{
		player->Roll += PLAYER_SPRITESHEET_COUNT;
	}
	else if (player->Roll >= PLAYER_SPRITESHEET_COUNT)
	{
		player->Roll -= PLAYER_SPRITESHEET_COUNT;
	}
}

void PlayerDraw(const struct Player *player)
{
	// Draw the character.
	int rollFrame = (int)floor(player->Roll);
	SDL_Rect src = {
		(rollFrame % PLAYER_SPRITESHEET_STRIDE) * PLAYER_SPRITESHEET_WIDTH,
		(rollFrame / PLAYER_SPRITESHEET_STRIDE) * PLAYER_SPRITESHEET_HEIGHT,
		PLAYER_SPRITESHEET_WIDTH,
		PLAYER_SPRITESHEET_HEIGHT
	};
	SDL_Rect dest = {
		(int) roundf(player->X * SCREEN_WIDTH / FIELD_WIDTH) - (PLAYER_SPRITESHEET_WIDTH / 2),
		(int) roundf(SCREEN_HEIGHT - player->Y * SCREEN_HEIGHT / FIELD_HEIGHT) - (PLAYER_SPRITESHEET_HEIGHT / 2),
		0,
		0
	};
	SDL_BlitSurface(PlayerSpritesheet, &src, Screen, &dest);
}

void PlayerReset(struct Player *player)
{
	player->X = FIELD_WIDTH / 2;
	player->Y = FIELD_HEIGHT - PLAYER_SIZE / 2;
	player->SpeedX = 0.0f;
	player->SpeedY = GRAVITY / 200 - FIELD_SCROLL / 200;
	player->AccelX = 0;
	player->Roll = 0.0f;
}