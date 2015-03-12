#include "sound.h"

#include <math.h>

#include "player.h"

#define BOUNCE_SPEED_MAX_VOLUME 3.0f


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
	SoundPlay(SoundPlayerBounce, (float)fabs(speed) / BOUNCE_SPEED_MAX_VOLUME);
}
