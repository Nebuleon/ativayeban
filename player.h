#pragma once

#include <stdbool.h>

#include <chipmunk/chipmunk.h>
#include <SDL.h>
#include <SDL_mixer.h>

typedef struct
{
	int Index;

	cpBody *Body;
	// Where the player is. (Center, meters.)
	float X, Y;

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

	SDL_Surface *Sprites;
} Player;

#define MAX_PLAYERS 2
extern Player players[MAX_PLAYERS];

extern SDL_Surface* PlayerSpritesheets[MAX_PLAYERS];
extern Mix_Chunk* SoundPlayerBounce;
extern int SoundPlayerRollChannel;

void PlayerUpdate(Player *player);
void PlayerDraw(const Player *player, const float y);
void PlayerInit(Player *player, const int i, const cpVect pos);
