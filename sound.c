#include "sound.h"

#include <math.h>

#include "player.h"
#include "utils.h"

#define BOUNCE_SPEED_MAX_VOLUME 150.0f
#define BOUNCE_SPEED_MIN_VOLUME 10.0f
#define ROLL_SPEED_MAX_VOLUME 20.0f
static int rollChannels[MAX_PLAYERS];
static int rollChannel = -1;

#define MUSIC_VOLUME_LOW 24
#define MUSIC_VOLUME_HIGH 64

Mix_Music *music;
Mix_Chunk* SoundPlayerRoll = NULL;


bool SoundLoad(void)
{
#define LOAD_SOUND(_sound, _path)\
	_sound = Mix_LoadWAV("data/sounds/" _path);\
	if (_sound == NULL)\
	{\
		printf("Mix_LoadWAV failed: %s\n", SDL_GetError());\
		SDL_ClearError();\
		return false;\
	}
	LOAD_SOUND(SoundPlayerRoll, "roll.ogg");

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		rollChannels[i] = -1;
	}

	return true;
}
void SoundFree(void)
{
	Mix_FreeChunk(SoundPlayerRoll);
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


void MusicSetLoud(const bool fullVolume)
{
	Mix_VolumeMusic(fullVolume ? MUSIC_VOLUME_HIGH : MUSIC_VOLUME_LOW);
}
