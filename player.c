#include <math.h>
#include <stdbool.h>
#include <stdint.h>

#include "draw.h"
#include "game.h"
#include "init.h"
#include "main.h"
#include "player.h"

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
}

void PlayerDraw(const struct Player *player)
{
	// Draw the character.
	DRAW_FillCircle(Screen, (int) roundf(player->X * SCREEN_WIDTH / FIELD_WIDTH), (int) roundf(SCREEN_HEIGHT - player->Y * SCREEN_HEIGHT / FIELD_HEIGHT), (int) ((PLAYER_SIZE / 2) * SCREEN_WIDTH / FIELD_WIDTH), SDL_MapRGB(Screen->format, 255, 255, 255));
}

void PlayerReset(struct Player *player)
{
	player->X = FIELD_WIDTH / 2;
	player->Y = FIELD_HEIGHT - PLAYER_SIZE / 2;
	player->SpeedX = 0.0f;
	player->SpeedY = GRAVITY / 200 - FIELD_SCROLL / 200;
	player->AccelX = 0;
}