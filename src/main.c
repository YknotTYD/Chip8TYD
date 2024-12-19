//main.c

#include "../include/chip8.h"
#include "../include/framebuffer.h"
#include <SDL3/SDL.h>
#define UNPACK2(value) ((value)[0]), ((value)[1])

//set framerate
//switch to SDL3
//fix wait_input making process unresponsive
//fix screen vlr being const
//fix the keys

static const int vlr=22;
static int screen_size[2]={64*vlr, 32*vlr};
static int launched = 1;
/*static const unsigned char keys[16]={
    sfKeyX, sfKeyNum1, sfKeyNum2, sfKeyNum3,
    sfKeyQ, sfKeyW, sfKeyE, sfKeyA, sfKeyS,
    sfKeyD, sfKeyZ, sfKeyC, sfKeyNum4, sfKeyR,
    sfKeyF, sfKeyV
};

static int f4_alt(void *args)
{
    sfRenderWindow *window = ((void **)args)[0];
    sfEvent *event = ((void **)args)[1];

    while (sfRenderWindow_pollEvent(window, event))
    {
        if ((*event).type == sfEvtClosed) {
            sfRenderWindow_close(window);
            return 0;
        }
    }
    return 1;
}

static int wait_for_input(void)
{
    while (1) {
        for (int i=0; i<16; i++) {
            if (sfKeyboard_isKeyPressed(keys[i])) {
                return i;
            }
        }
    }
}

static void update_keys(unsigned char (*keypad)[16])
{
    for (int i=0; i<16; i++) {
        (*keypad)[i]=sfKeyboard_isKeyPressed(keys[i]);
    }
 }

static void draw_chip(Chip8 *chip, FrameBuffer *fbuffer, int vlr)
{
    for (int y=0; y<32; y++) {
        for (int x=0; x<64; x++) {

            if (!chip->frame_buffer[y*64+x]) {
                continue;
            }

            draw_square(
                fbuffer, x*vlr, y*vlr, vlr, vlr,
                (char[4]){20, 200, 32, 255}
            );
        }
    }
}*/

static SDL_Window *window = 0;
static SDL_Renderer *renderer = 0;
static const bool *keyboard;

static const unsigned char keys[16]={
    SDL_SCANCODE_X, SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3,
    SDL_SCANCODE_Q, SDL_SCANCODE_W, SDL_SCANCODE_E, SDL_SCANCODE_A,
    SDL_SCANCODE_S, SDL_SCANCODE_D, SDL_SCANCODE_Z, SDL_SCANCODE_C,
    SDL_SCANCODE_4, SDL_SCANCODE_R, SDL_SCANCODE_F, SDL_SCANCODE_V
};

static int wait_for_input()
{
    while (1) {
        keyboard = SDL_GetKeyboardState(0);
        for (int i = 0; i < 16; i++) {
            if (keyboard[keys[i]]) {
                return i;
            }
        }
    }
}

static void update_keys(unsigned char (*keypad)[16])
{
    keyboard = SDL_GetKeyboardState(0);

    for (int i = 0; i < 16; i++) {
        (*keypad)[i]=keyboard[keys[i]];
    }
    return;
}

static int ch8_cpu_inf_loop_fallback(void *args)
{

    SDL_Event *event = args;

    while (SDL_PollEvent(event)) {
        if (event->type == SDL_EVENT_QUIT) {
            launched = 0;
            return 0;
        }
    }
    return 1;
}

static void draw_chip(Chip8 *chip, FrameBuffer *fbuffer, int vlr)
{
    for (int y = 0; y < 32; y++) {
        for (int x = 0; x < 64; x++) {

            if (!chip->frame_buffer[y * 64 + x]) {
                continue;
            }

            draw_square(fbuffer, x * vlr, y * vlr, vlr, vlr, 0x14c820FF);
        }
    }
}


int main(int argc, char **argv)
{

    if (argc < 2) {
        return 84;
    }

    static Chip8 *chip;

    Chip8Utils.InitChip(&chip, wait_for_input, update_keys);
    Chip8Utils.LoadChip(chip, argv[1]);

    Chip8Utils.set_seed(time(NULL));

    FrameBuffer *fbuffer = new_frame_buffer(UNPACK2(screen_size));
    clear_buffer(fbuffer);

    SDL_Event event;

    SDL_Init(SDL_INIT_VIDEO);

    SDL_CreateWindowAndRenderer("Window Name?", UNPACK2(screen_size), 0, &window, &renderer);
    SDL_SetRenderDrawColor(renderer, 22, 22, 22, 255);

    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR32, SDL_TEXTUREACCESS_STREAMING,
        fbuffer->width, fbuffer->height);

    while (launched) {

        while (SDL_GetKeyboardFocus() != window);

        keyboard = SDL_GetKeyboardState(0);
        ch8_cpu_inf_loop_fallback(&event);

        Chip8Utils.ProcessFrame(chip, ch8_cpu_inf_loop_fallback, &event);

        clear_buffer(fbuffer);
        draw_chip(chip, fbuffer, vlr);

        SDL_RenderClear(renderer);
        SDL_UpdateTexture(texture, 0, fbuffer->pixels, fbuffer->width * sizeof(int));

        SDL_RenderTexture(renderer, texture, 0, 0);
        SDL_RenderPresent(renderer);

    }

    Chip8Utils.FreeChip(chip);
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);

    return 0;
}
