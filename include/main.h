//header.h

#ifndef MAINDEF
    #define MAINDEF

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "chip8.h"
#include "framebuffer.h"
#include "audio.h"
//#include <SDL3/SDL.h>
//#include <SDL3_ttf/SDL_ttf.h>

#define UNPACK2(value) ((value)[0]), ((value)[1])

int read_file(char **kronk_buffer, char *filename);
void DrawText(SDL_Renderer *renderer);

static const unsigned char keys[16] = {
    SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3, SDL_SCANCODE_4,
    SDL_SCANCODE_Q, SDL_SCANCODE_W, SDL_SCANCODE_E, SDL_SCANCODE_R,
    SDL_SCANCODE_A, SDL_SCANCODE_S, SDL_SCANCODE_D, SDL_SCANCODE_F,
    SDL_SCANCODE_Z, SDL_SCANCODE_X, SDL_SCANCODE_C, SDL_SCANCODE_V
};

#endif
