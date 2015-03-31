#pragma once

#include <stdbool.h>

#include <chipmunk/chipmunk.h>
#include <SDL.h>
#include <SDL_mixer.h>

typedef struct
{
	cpBody *Body;
	// Where the player is. (Center, meters.)
	float X, Y;
	// Where the player is going. (Meters per second.)
	float SpeedX, SpeedY;

	// The last value returned by GetMovement.
	int16_t AccelX;

	// Used to detect rolling for rolling sound
	bool WasOnSurface;

	// Used to animate the player rolling
	int Roll;

	// Blink until counter runs out
	int BlinkCounter;
	// Blink when counter reaches zero
	int NextBlinkCounter;
} Player;

extern SDL_Surface* PlayerSpritesheet;
extern Mix_Chunk* SoundPlayerBounce;
extern Mix_Chunk* SoundPlayerRoll;
extern int SoundPlayerRollChannel;

void PlayerUpdate(Player *player);
void PlayerDraw(const Player *player, const float y);
void PlayerInit(Player *player);
