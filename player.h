#ifndef _PLAYER_H_
#define _PLAYER_H_

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

	// Blink until counter runs out
	int BlinkCounter;
	// Blink when counter reaches zero
	int NextBlinkCounter;
};

extern SDL_Surface* PlayerSpritesheet;

extern void PlayerUpdate(struct Player *player);
extern void PlayerDraw(const struct Player *player);
extern void PlayerReset(struct Player *player);

#endif /* !defined(_PLAYER_H_) */
