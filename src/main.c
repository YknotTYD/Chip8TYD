//main.c

#include "../include/main.h"

//set framerate
//switch to SDL3
//fix wait_input making process unresponsive
//fix screen vlr being const
//a sound network
//put most below static functions in a .c

static const int vlr=22;
static double FPS = 60.0;
static int CCPF = 10;
static int screen_size[2]={64*vlr, 32*vlr};
static int launched = 1;
static SDL_Window *window = 0;
static SDL_Renderer *renderer = 0;
static SDL_Event event;
static const bool *keyboard;

static int ch8_cpu_inf_loop_fallback(void *args)
{

    SDL_Event *events = args;

    while (SDL_PollEvent(events)) {
        if (events->type == SDL_EVENT_QUIT) {
            launched = 0;
            return 0;
        }
    }
    return 1;
}

static int wait_for_input(void)
{
    while (1) {
        keyboard = SDL_GetKeyboardState(0);
        for (int i = 0; i < 16; i++) {
            if (keyboard[keys[i]]) {
                return i;
            }
        }
        if (!ch8_cpu_inf_loop_fallback(&event)) {
            break;
        }
    }
    return -1;
}

static void update_keys(unsigned char (*keypad)[16])
{
    keyboard = SDL_GetKeyboardState(0);
    for (int i = 0; i < 16; i++) {
        (*keypad)[i]=keyboard[keys[i]];
    }
    return;
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

static void render(SDL_Texture *texture, FrameBuffer *fbuffer)
{
    SDL_UpdateTexture(texture, 0, fbuffer->pixels, fbuffer->width * sizeof(int));
    SDL_RenderClear(renderer);

    SDL_RenderTexture(renderer, texture, 0, 0);
    SDL_RenderPresent(renderer);
    return;
}

int main(int argc, char **argv)
{

    if (argc < 2) {
        return 84;
    }

    double frame_start;
    static Chip8 *chip;

    Chip8Utils.InitChip(&chip, wait_for_input, update_keys);
    Chip8Utils.LoadChip(chip, argv[1]);
    Chip8Utils.set_seed(time(NULL));

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

    FrameBuffer *fbuffer = new_frame_buffer(UNPACK2(screen_size));
    stream_t *stream = LoadAudio("files/audio/500.wav");

    SDL_CreateWindowAndRenderer("ChipTYD", UNPACK2(screen_size), 0, &window, &renderer);
    SDL_Texture *texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ABGR32, SDL_TEXTUREACCESS_STREAMING,
        fbuffer->width, fbuffer->height
    );

    while (launched) {

        while (SDL_GetKeyboardFocus() != window) {
            ch8_cpu_inf_loop_fallback(&event);
        }

        frame_start = NOW;

        ch8_cpu_inf_loop_fallback(&event);
        keyboard = SDL_GetKeyboardState(0);

        Chip8Utils.ProcessCycles(chip, CCPF);

        if (chip->sound_timer) {
            Unpause(stream);
        } else {
            Pause(stream);
        }

        clear_buffer(fbuffer);
        draw_chip(chip, fbuffer, vlr);

        render(texture, fbuffer);

        while ((NOW - frame_start) < (1 / FPS));
    }

    Chip8Utils.FreeChip(chip);
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    free_stream(stream);

    return 0;
}


