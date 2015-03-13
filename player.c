#include <math.h>
#include <stdbool.h>
#include <stdint.h>

#include "draw.h"
#include "game.h"
#include "init.h"
#include "main.h"
#include "player.h"
#include "sound.h"

#define PLAYER_SPRITESHEET_WIDTH 35
#define PLAYER_SPRITESHEET_HEIGHT 35
#define PLAYER_SPRITESHEET_STRIDE 8
#define PLAYER_SPRITESHEET_COUNT 16
#define PLAYER_ROLL_SCALE 0.015f
#define PLAYER_BLINK_FRAME_OFFSET 16
#define PLAYER_BLINK_FRAMES 200
#define PLAYER_BLINK_INTERVAL_FRAMES ((rand() % 500) + 500)
#define PLAYER_BLINK_CHANCE 50

SDL_Surface* PlayerSpritesheet = NULL;
Mix_Chunk* SoundPlayerBounce = NULL;
Mix_Chunk* SoundPlayerRoll = NULL;

void PlayerUpdate(struct Player *player)
{
	// Update the ball scroll.
	player->Y += FIELD_SCROLL / 1000;

	// Update the speed at which the player is going.
	player->SpeedX += ((float) player->AccelX / 32767.0f) * ACCELERATION / 1000;

	// Update the player's position and speed.
	bool bounce = false;

	// Left and right edges (X). If the horizontal speed would run the
	// ball into an edge, use up some of the energy in the impact and
	// rebound the ball.
	if (player->SpeedX < 0 && player->X - PLAYER_SIZE / 2 + player->SpeedX / 1000 < 0)
	{
		player->X = PLAYER_SIZE / 2
			+ ((player->X - PLAYER_SIZE / 2) - (player->SpeedX / 1000)) * FIELD_REBOUND;
		player->SpeedX = -player->SpeedX * FIELD_REBOUND;
		bounce = true;
	}
	else if (player->SpeedX > 0 && player->X + PLAYER_SIZE / 2 + player->SpeedX / 1000 > FIELD_WIDTH)
	{
		player->X = FIELD_WIDTH - PLAYER_SIZE / 2
			+ (FIELD_WIDTH - (player->X + PLAYER_SIZE / 2) - (player->SpeedX / 1000)) * FIELD_REBOUND;
		player->SpeedX = -player->SpeedX * FIELD_REBOUND;
		bounce = true;
	}
	else
	{
		player->X += player->SpeedX / 1000;
	}

	if (bounce)
	{
		SoundPlayBounce(player->SpeedX);
	}

	if (player->OnSurface)
	{
		// If on surface, apply friction to the player's speed (X).
		player->SpeedX *= 1.0f - FRICTION;
		SoundPlayRoll(player->SpeedX);
	}
	else
	{
		// If not, apply gravity (Y).
		player->SpeedY += GRAVITY / 1000;
		player->Y += player->SpeedY / 1000;
		SoundStopRoll();
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

	// Randomly blink after not blinking for a while
	player->BlinkCounter--;
	player->NextBlinkCounter--;
	if (player->NextBlinkCounter <= 0)
	{
		player->BlinkCounter = PLAYER_BLINK_FRAMES;
		player->NextBlinkCounter = PLAYER_BLINK_INTERVAL_FRAMES;
	}
}

void PlayerDraw(const struct Player *player)
{
	// Draw the character.
	int rollFrame = (int)floor(player->Roll);
	if (player->BlinkCounter > 0)
	{
		rollFrame += PLAYER_BLINK_FRAME_OFFSET;
	}
	SDL_Rect src = {
		(Sint16)((rollFrame % PLAYER_SPRITESHEET_STRIDE) * PLAYER_SPRITESHEET_WIDTH),
		(Sint16)((rollFrame / PLAYER_SPRITESHEET_STRIDE) * PLAYER_SPRITESHEET_HEIGHT),
		PLAYER_SPRITESHEET_WIDTH,
		PLAYER_SPRITESHEET_HEIGHT
	};
	SDL_Rect dest = {
		(Sint16)(SCREEN_X(player->X) - PLAYER_SPRITESHEET_WIDTH / 2),
		(Sint16)(SCREEN_Y(player->Y) - PLAYER_SPRITESHEET_HEIGHT / 2),
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
	player->OnSurface = false;
	player->BlinkCounter = 0;
	player->NextBlinkCounter = 1;
}