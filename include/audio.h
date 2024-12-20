//aucio.h

#include <SDL3/SDL.h>
#include <stdlib.h>

#ifndef CHIP8TYDAUDIO
    #define CHIP8TYDAUDIO

typedef struct {
    uint8_t *audio_buff;
    uint32_t audio_lenght;
} AudioData;

typedef struct {
    uint8_t *buff;
    SDL_AudioStream *stream;
    AudioData *data;
} stream_t;

stream_t *LoadAudio(const char *filepath);
void Pause(stream_t *stream);
void Unpause(stream_t *stream);
void free_stream(stream_t *stream);

#endif
