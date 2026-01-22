#ifndef AUDIOPLAYER
#define AUDIOPLAYER

#include <miniaudio.h>

typedef struct {
    ma_engine soundEngine;
} AudioPlayer;

void AudioPlayer_Init(AudioPlayer* pAudio);
void AudioPlayer_Terminate(AudioPlayer* pAudio);

void AudioPlayer_Play2DSound(AudioPlayer* pAudio, const char* path);

#endif