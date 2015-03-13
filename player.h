#pragma once

#include <stdbool.h>

#include <SDL.h>
#include <SDL_mixer.h>

struct Player
{
	// Where the player is. (Center, meters.)
	float X, Y;
	// Where the player is going. (Meters per second.)
	float SpeedX, SpeedY;

	// The last value returned by GetMovement.
	int16_t AccelX;

	// Used to animate the player rolling
	float Roll;

	// Whether player is on a surface
	bool OnSurface;

	// Blink until counter runs out
	int BlinkCounter;
	// Blink when counter reaches zero
	int NextBlinkCounter;
};

extern SDL_Surface* PlayerSpritesheet;
extern Mix_Chunk* SoundPlayerBounce;
extern Mix_Chunk* SoundPlayerRoll;
extern int SoundPlayerRollChannel;

extern void PlayerUpdate(struct Player *player);
extern void PlayerDraw(const struct Player *player);
extern void PlayerReset(struct Player *player);
