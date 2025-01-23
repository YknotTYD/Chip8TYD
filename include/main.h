//main.h

#include "chip8.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdlib.h>
#include <unistd.h>

#ifndef MAIN_HEADER
    #define MAIN_HEADER
    #define UNPACK2(arr) arr[0], arr[1]

typedef struct {
    Chip8 *chip;
    TTF_Font *font;
    SDL_Window *win;
    SDL_Renderer *ren;
    SDL_Event events;
    int quit;
    char *filepath;
} context_t;

static const unsigned char keys[16] = {
    SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3, SDL_SCANCODE_4,
    SDL_SCANCODE_Q, SDL_SCANCODE_W, SDL_SCANCODE_E, SDL_SCANCODE_R,
    SDL_SCANCODE_A, SDL_SCANCODE_S, SDL_SCANCODE_D, SDL_SCANCODE_F,
    SDL_SCANCODE_Z, SDL_SCANCODE_X, SDL_SCANCODE_C, SDL_SCANCODE_V
};

#endif
