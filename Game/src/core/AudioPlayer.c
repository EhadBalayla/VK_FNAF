#define MINIAUDIO_IMPLEMENTATION
#include "AudioPlayer.h"

#include <stdio.h>

void AudioPlayer_Init(AudioPlayer* pAudio) {
    ma_result result = ma_engine_init(NULL, &pAudio->soundEngine);
	if (result != MA_SUCCESS) {
		fprintf(stderr, "FAILED TO INITIALIZE AUDIO\n");
        exit(EXIT_FAILURE);
	}
}
void AudioPlayer_Terminate(AudioPlayer* pAudio) {
    ma_engine_uninit(&pAudio->soundEngine); 
}

void AudioPlayer_Play2DSound(AudioPlayer* pAudio, const char* path) {
    ma_engine_play_sound(&pAudio->soundEngine, path, NULL);
}