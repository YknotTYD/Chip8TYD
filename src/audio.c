//aucio.c

#include "../include/audio.h"

//make an AudioUtils
//rename AudioData

static void AudioStreamCallback(void *userdata, SDL_AudioStream *stream, int additional_amount, int total_amount) {

    additional_amount = additional_amount;
    total_amount = total_amount;

    AudioData *data = (AudioData *)userdata;
    
    while ((uint32_t)SDL_GetAudioStreamAvailable(stream) < data->audio_lenght) {
        SDL_PutAudioStreamData(stream, data->audio_buff, data->audio_lenght);
    }
}

static SDL_AudioStream *LoopStream(const char *filepath, uint8_t **audio_buff, uint32_t *audio_lenght, AudioData **audio_data)
{
    SDL_LoadWAV(filepath, &(SDL_AudioSpec){SDL_AUDIO_UNKNOWN, 1, 240}, audio_buff, audio_lenght);
    SDL_AudioSpec spec = {SDL_AUDIO_S16, 1, 44100};
    SDL_AudioStream *stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, NULL, NULL);
    SDL_PutAudioStreamData(stream, *audio_buff, *audio_lenght);
    SDL_ResumeAudioDevice(SDL_GetAudioStreamDevice(stream));
    *audio_data = malloc(sizeof(AudioData));
    (*audio_data)->audio_buff =  *audio_buff;
    (*audio_data)->audio_lenght =  *audio_lenght;
    SDL_SetAudioStreamGetCallback(stream, AudioStreamCallback, *audio_data);
    return stream;
}

stream_t *LoopAudio(const char *filepath)
{
    uint8_t *audio_buff;
    uint32_t audio_lenght;
    AudioData *audio_data;
    SDL_AudioStream *stream;
    stream_t *audio;

    stream = LoopStream(
        filepath, &audio_buff,
        &audio_lenght, &audio_data
    );

    audio = malloc(sizeof(stream_t));
    audio->buff = audio_buff;
    audio->data = audio_data;
    audio->stream = stream;

    return audio;
}

void Pause(stream_t *stream)
{
    SDL_PauseAudioStreamDevice(stream->stream);
    return;
}

void Unpause(stream_t *stream)
{
    SDL_ResumeAudioStreamDevice(stream->stream);
    return;
}

void free_stream(stream_t *stream)
{
    SDL_DestroyAudioStream(stream->stream);
    SDL_free(stream->buff);
    free(stream->data);
    return;
}
