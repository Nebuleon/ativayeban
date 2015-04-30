#pragma once

#include <stdbool.h>

#include <chipmunk/chipmunk.h>
#include <SDL.h>
#include <SDL_mixer.h>

typedef struct
{
	int Index;

	// Whether the player is in the game or not
	bool Enabled;
	bool Alive;
	// Once dead, wait this long before respawning
	int RespawnCounter;

	cpBody *Body;
	// Cached positions for drawing only
	float x, y;

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

void PlayerUpdate(Player *player, const Uint32 ms);
void PlayerDraw(const Player *player, const float y);
void PlayerInit(Player *player, const int i, const cpVect pos);

void PlayerKill(Player *player);
void PlayerRespawn(Player *player, const float x, const float y);
void PlayerRevive(Player *player);

int PlayerAliveCount(void);