#pragma once

#include <SDL_mixer.h>

void SoundPlay(Mix_Chunk *sound, const float volume);
void SoundPlayBounce(const float speed);
