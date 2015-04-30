#include <math.h>
#include <stdbool.h>
#include <stdint.h>

#include "draw.h"
#include "game.h"
#include "init.h"
#include "main.h"
#include "player.h"
#include "space.h"
#include "sound.h"
#include "utils.h"

#define PLAYER_SPRITESHEET_WIDTH 35
#define PLAYER_SPRITESHEET_HEIGHT 35
#define PLAYER_SPRITESHEET_STRIDE 8
#define PLAYER_SPRITESHEET_COUNT 16
#define PLAYER_ROLL_SCALE 0.015f
#define PLAYER_BLINK_FRAME_OFFSET 16
#define PLAYER_BLINK_FRAMES 20
#define PLAYER_BLINK_INTERVAL_FRAMES ((rand() % 100) + 100)
#define PLAYER_BLINK_CHANCE 50

SDL_Surface* PlayerSpritesheets[MAX_PLAYERS];
Mix_Chunk* SoundPlayerBounce = NULL;

Player players[MAX_PLAYERS];

typedef struct
{
	cpVect BounceForce;
	int Num;
} OnArbiterData;
static void OnArbiter(cpBody *body, cpArbiter *arb, void *data);
void PlayerUpdate(Player *player)
{
	// Update the speed at which the player is going.
	// Provide positive bonus for:
	// - when the player is very slow
	// - when the player is rolling
	// And negative bonus for:
	// - when the player is above max speed
	float accel = ACCELERATION;
	const cpVect vel = cpBodyGetVelocity(player->Body);
	const float relVelX = (float)fabs(vel.x) * SIGN(vel.x * player->AccelX);
	if (relVelX > MAX_SPEED) accel = ACCELERATION_MAX_SPEED;
	if (relVelX < MIN_SPEED) accel += MIN_SPEED_ACCEL_BONUS;
	if (player->WasOnSurface) accel += ROLL_ACCEL_BONUS;
	cpBodySetForce(player->Body, cpv(player->AccelX / 32767.0f * accel, 0));
	//printf("%f\n", vel.x);

	// Detect bounces
	OnArbiterData o = { cpvzero, 0 };
	cpBodyEachArbiter(player->Body, OnArbiter, &o);
	if (o.Num > 0)
	{
		if (!cpveql(o.BounceForce, cpvzero))
		{
			SoundPlayBounce((float)cpvlength(o.BounceForce));
		}
		if (player->WasOnSurface)
		{
			float angularV = (float)cpBodyGetAngularVelocity(player->Body);
			//printf("angularV %f\n", angularV);
			SoundPlayRoll(player->Index, angularV);
		}
		player->WasOnSurface = true;
	}
	else
	{
		SoundStopRoll(player->Index);
		player->WasOnSurface = false;
	}

	player->Roll = (int)(-cpBodyGetAngle(player->Body) / (2 * M_PI) * PLAYER_SPRITESHEET_COUNT);
	player->Roll = player->Roll % PLAYER_SPRITESHEET_COUNT;
	if (player->Roll < 0) player->Roll += PLAYER_SPRITESHEET_COUNT;

	// Randomly blink after not blinking for a while
	player->BlinkCounter--;
	player->NextBlinkCounter--;
	if (player->NextBlinkCounter <= 0)
	{
		player->BlinkCounter = PLAYER_BLINK_FRAMES;
		player->NextBlinkCounter = PLAYER_BLINK_INTERVAL_FRAMES;
	}
}
static void OnArbiter(cpBody *body, cpArbiter *arb, void *data)
{
	UNUSED(body);
	OnArbiterData *o = data;
	o->BounceForce = cpvadd(o->BounceForce, cpArbiterTotalImpulse(arb));
	o->Num++;
}

void PlayerDraw(const Player *player, const float y)
{
	// Draw the character.
	int rollFrame = player->Roll;
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

	const cpVect pos = cpBodyGetPosition(player->Body);
	SDL_Rect dest = {
		(Sint16)(SCREEN_X(pos.x) - PLAYER_SPRITESHEET_WIDTH / 2),
		(Sint16)(SCREEN_Y(pos.y) - PLAYER_SPRITESHEET_HEIGHT / 2 - y),
		0,
		0
	};
	SDL_BlitSurface(player->Sprites, &src, Screen, &dest);
}

void PlayerInit(Player *player, const int i, const cpVect pos)
{
	player->Index = i;
	player->Enabled = false;
	player->Body = cpSpaceAddBody(
		space.Space,
		cpBodyNew(10.0f, cpMomentForCircle(10.0f, 0.0f, PLAYER_RADIUS, cpvzero)));
	cpBodySetPosition(player->Body, pos);
	cpShape *shape = cpSpaceAddShape(
		space.Space, cpCircleShapeNew(player->Body, PLAYER_RADIUS, cpvzero));
	cpShapeSetElasticity(shape, PLAYER_ELASTICITY);
	cpShapeSetFriction(shape, 0.9f);
	player->AccelX = 0;
	player->WasOnSurface = false;
	player->Roll = 0;
	player->BlinkCounter = 0;
	player->NextBlinkCounter = 1;
	player->Sprites = PlayerSpritesheets[i];
}

void PlayerDisable(Player *player)
{
	if (player->Enabled)
	{
		SoundPlay(SoundLose, 1.0);
	}
	player->Enabled = false;
	SoundStopRoll(player->Index);
}

int PlayerEnabledCount(void)
{
	int num = 0;
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		if (!players[i].Enabled) continue;
		num++;
	}
	return num;
}
