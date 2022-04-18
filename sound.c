#include "sound.h"

#include <math.h>

#include "player.h"
#include "sys_config.h"
#include "utils.h"

#define BOUNCE_SPEED_MAX_VOLUME 150.0f
#define BOUNCE_SPEED_MIN_VOLUME 10.0f
#define ROLL_SPEED_MAX_VOLUME 20.0f
static int rollChannels[MAX_PLAYERS];
static int callChannels[MAX_PLAYERS];

#define MUSIC_VOLUME_LOW 24
#define MUSIC_VOLUME_HIGH 64

Mix_Music *music;
Mix_Chunk* SoundPlayerRoll = NULL;
Mix_Chunk *SoundPlayerCalls[MAX_PLAYERS];


bool SoundLoad(void)
{
#define LOAD_SOUND(_sound, _path)\
	_sound = Mix_LoadWAV(DATA_DIR "sounds/" _path);\
	if (_sound == NULL)\
	{\
		printf("Mix_LoadWAV failed: %s\n", SDL_GetError());\
		SDL_ClearError();\
		return false;\
	}
	LOAD_SOUND(SoundPlayerRoll, "roll.ogg");
	LOAD_SOUND(SoundPlayerCalls[0], "penguin.ogg");
	LOAD_SOUND(SoundPlayerCalls[1], "chicken.ogg");
	LOAD_SOUND(SoundPlayerCalls[2], "quack.ogg");
	LOAD_SOUND(SoundPlayerCalls[3], "turkey.ogg");

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		rollChannels[i] = -1;
		callChannels[i] = -1;
	}

	return true;
}
void SoundFree(void)
{
	Mix_FreeChunk(SoundPlayerRoll);
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		Mix_FreeChunk(SoundPlayerCalls[i]);
	}
}

void SoundPlay(Mix_Chunk *sound, const float volume)
{
	const int channel = Mix_PlayChannel(-1, sound, 0);
	if (channel >= 0)
	{
		Mix_Volume(channel, (int)round(MIN(1.0f, volume) * MIX_MAX_VOLUME));
	}
}

void SoundPlayBounce(const float speed)
{
	const float imp = (float)fabs(speed);
	if (imp < BOUNCE_SPEED_MIN_VOLUME) return;
	SoundPlay(SoundPlayerBounce, imp / BOUNCE_SPEED_MAX_VOLUME);
}

void SoundPlayRoll(const int player, const float speed)
{
	if (rollChannels[player] == -1)
	{
		rollChannels[player] = Mix_PlayChannel(-1, SoundPlayerRoll, -1);
	}
	if (rollChannels[player] != -1)
	{
		const float volume = (float)fabs(speed) / ROLL_SPEED_MAX_VOLUME;
		Mix_Volume(
			rollChannels[player],
			(int)round(MIN(1.0f, volume) * MIX_MAX_VOLUME));
	}
}

void SoundStopRoll(const int player)
{
	if (rollChannels[player] != -1)
	{
		Mix_HaltChannel(rollChannels[player]);
		rollChannels[player] = -1;
	}
}

void SoundPlayCall(const int player)
{
	if (callChannels[player] == -1)
	{
		callChannels[player] =
			Mix_PlayChannel(-1, SoundPlayerCalls[player], -1);
	}
	if (callChannels[player] != -1)
	{
		Mix_Volume(callChannels[player], MIX_MAX_VOLUME);
	}
}
void SoundStopCall(const int player)
{
	if (callChannels[player] != -1)
	{
		Mix_HaltChannel(callChannels[player]);
		callChannels[player] = -1;
	}
}

void MusicSetLoud(const bool fullVolume)
{
	Mix_VolumeMusic(fullVolume ? MUSIC_VOLUME_HIGH : MUSIC_VOLUME_LOW);
}
