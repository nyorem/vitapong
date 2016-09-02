#ifndef _AUDIO_H_
#define _AUDIO_H_

#include <psp2/audioout.h>

int openPort (int size, int freq, int mode) {
    return sceAudioOutOpenPort(SCE_AUDIO_OUT_PORT_TYPE_BGM, size, freq, mode);
}

void setVolume (int port, int vol) {
    sceAudioOutSetVolume(port, SCE_AUDIO_VOLUME_FLAG_L_CH |SCE_AUDIO_VOLUME_FLAG_R_CH, (int[]){vol,vol});
}

void output (int port, void* buffer) {
    sceAudioOutOutput(port, buffer);
}

void releasePort (int port) {
    sceAudioOutReleasePort(port);
}

#endif

