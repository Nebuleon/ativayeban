#pragma once

#include <SDL_mixer.h>

void SoundPlay(Mix_Chunk *sound, const float volume);
void SoundPlayBounce(const float speed);
void SoundPlayRoll(const float speed);
void SoundStopRoll(void);
