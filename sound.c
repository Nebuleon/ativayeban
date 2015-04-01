#include "sound.h"

#include <math.h>

#include "player.h"

#define BOUNCE_SPEED_MAX_VOLUME 150.0f
#define BOUNCE_SPEED_MIN_VOLUME 10.0f
#define ROLL_SPEED_MAX_VOLUME 20.0f
static int rollChannel = -1;

#define MUSIC_VOLUME_LOW 24
#define MUSIC_VOLUME_HIGH 80

Mix_Music *music;


void SoundPlay(Mix_Chunk *sound, const float volume)
{
	const int channel = Mix_PlayChannel(-1, sound, 0);
	if (channel >= 0)
	{
		Mix_Volume(channel, (int)round(min(1.0f, volume) * MIX_MAX_VOLUME));
	}
}

void SoundPlayBounce(const float speed)
{
	const float imp = (float)fabs(speed);
	if (imp < BOUNCE_SPEED_MIN_VOLUME) return;
	SoundPlay(SoundPlayerBounce, imp / BOUNCE_SPEED_MAX_VOLUME);
}

void SoundPlayRoll(const float speed)
{
	if (rollChannel == -1)
	{
		rollChannel = Mix_PlayChannel(-1, SoundPlayerRoll, -1);
	}
	if (rollChannel != -1)
	{
		const float volume = (float)fabs(speed) / ROLL_SPEED_MAX_VOLUME;
		Mix_Volume(rollChannel, (int)round(min(1.0f, volume) * MIX_MAX_VOLUME));
	}
}

void SoundStopRoll(void)
{
	if (rollChannel != -1)
	{
		Mix_HaltChannel(rollChannel);
		rollChannel = -1;
	}
}


void MusicSetLoud(const bool fullVolume)
{
	Mix_VolumeMusic(fullVolume ? MUSIC_VOLUME_HIGH : MUSIC_VOLUME_LOW);
}
