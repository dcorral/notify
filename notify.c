#include "alarm.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

volatile int keepPlaying = 1;

void handleSigint(int sig) {
    keepPlaying = 0;
    SDL_CloseAudio();
    SDL_Quit();
}

void playSound() {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "Could not initialize SDL: %s\n", SDL_GetError());
        return;
    }

    SDL_AudioSpec wavSpec;
    Uint32 wavLength = alarm_wav_len;
    Uint8 *wavBuffer = alarm_wav;

    if (SDL_LoadWAV_RW(SDL_RWFromMem(wavBuffer, wavLength), 1, &wavSpec, &wavBuffer, &wavLength) == NULL) {
        fprintf(stderr, "Could not load WAV: %s\n", SDL_GetError());
        SDL_Quit();
        return;
    }

    if (SDL_OpenAudio(&wavSpec, NULL) < 0) {
        fprintf(stderr, "Could not open audio: %s\n", SDL_GetError());
        SDL_FreeWAV(wavBuffer);
        SDL_Quit();
        return;
    }

    SDL_PauseAudio(0);

    Uint32 audio_len = wavLength;
    Uint8 *audio_pos = wavBuffer;

    while (audio_len > 0 && keepPlaying) {
        int len = (audio_len > 4096) ? 4096 : audio_len;
        SDL_QueueAudio(1, audio_pos, len);
        audio_pos += len;
        audio_len -= len;
        SDL_Delay(100);
    }

    SDL_CloseAudio();
    SDL_FreeWAV(wavBuffer);
    SDL_Quit();
}

int main(int argc, char *argv[]) {
    // Register the signal handler for SIGINT (Ctrl+C)
    signal(SIGINT, handleSigint);

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <command>\n", argv[0]);
        return 1;
    }

    char command[1024] = {0};
    for (int i = 1; i < argc; ++i) {
        strcat(command, argv[i]);
        strcat(command, " ");
    }

    int status = system(command);

    if (status == 0) {
        printf("Command finished successfully!\n");
        playSound();
    } else {
        printf("Command failed with status %d!\n", status);
        playSound();
    }

    return status;
}

