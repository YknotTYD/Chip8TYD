//main.c

#include "../include/main.h"
//#include <emscripten.h>

static const int vlr=15;
static double FPS = 60.0; //TODO: fix dt/st FPS fuck up
static int CCPF = 10;
static int screen_size[2]={1650, 900};
static int chip_edge = 3;
static int key_size = 32;
static int key_gap = 5;

static int chip_screen_size[2]={64 * vlr, 32 * vlr};
static const unsigned char *keyboard;
static context_t context;

//compile with font
//don't create and destroy it every time
//display full ASM repr
//add menus with easing function
//display FPS
//move most of main.c away from main.c
//add a keypad that lights up as you press keys
//fix emulation speed && FPS fuck up
//make the window resizable
//make static consts macros
//fix the 2 key being fucked up
//add keys to keys
//use event to handle the keys instead

static int ch8_cpu_inf_loop_fallback(void)
{
    while (SDL_PollEvent(&context.events)) {
        if (context.events.type == SDL_QUIT) {
            context.quit = 1;
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
        if (ch8_cpu_inf_loop_fallback() == 0) {
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

static void draw_chip(context_t *context)
{
    SDL_SetRenderDrawColor(context->ren, 0, 222, 0, 255);

    for (int y = 0; y < 32; y++) {
        for (int x = 0; x < 64; x++) {

            if (!context->chip->frame_buffer[y * 64 + x]) {
                continue;
            }

            SDL_RenderFillRect(
                context->ren,
                &(SDL_Rect){CHIPLEFT + vlr * x,
                CHIPTOP + vlr * y, vlr, vlr}
            );
        }
    }

    return;
}

static void draw_edge(context_t *context)
{
    SDL_SetRenderDrawColor(context->ren, 55, 55, 55, 255);

    SDL_RenderFillRect(context->ren, &(SDL_Rect){
        CHIPLEFT - chip_edge,
        CHIPTOP - chip_edge,
        chip_edge,
        chip_screen_size[1] + chip_edge
    });

    SDL_RenderFillRect(context->ren, &(SDL_Rect){
        CHIPLEFT - chip_edge,
        CHIPTOP - chip_edge,
        chip_screen_size[0] + chip_edge,
        chip_edge
    });

    SDL_RenderFillRect(context->ren, &(SDL_Rect){
        CHIPLEFT - chip_edge,
        CHIPTOP + chip_screen_size[1],
        chip_screen_size[0] + chip_edge,
        chip_edge
    });

    SDL_RenderFillRect(context->ren, &(SDL_Rect){
        CHIPLEFT + chip_screen_size[0],
        CHIPTOP - chip_edge,
        chip_edge,
        chip_screen_size[1] + chip_edge * 2
    });

    return;
}

static void init_context(context_t *context)
{
    context->win = SDL_CreateWindow("Chip8TYD", 100, 75, UNPACK2(screen_size), SDL_WINDOW_SHOWN);
    context->ren = SDL_CreateRenderer(context->win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    context->quit = 0;
    context->font = TTF_OpenFont("assets/fonts/SourceCodePro-Medium.ttf", 30);

    return;
}

static void draw_filename(context_t *context)
{
    int width;
    int height;

    SDL_Surface *surf = TTF_RenderText_Blended_Wrapped(context->font, context->filepath,
        (SDL_Color){255, 255, 0, 255}, screen_size[0] - 10);
    SDL_Texture *text = SDL_CreateTextureFromSurface(context->ren, surf);

    SDL_QueryTexture(text, 0, 0, &width, &height);

    SDL_RenderCopy(
        context->ren, text, 0,
        &(SDL_Rect){
        (screen_size[0] - width) / 2, (CHIPTOP - height) / 2,
        width, height}
    );

    SDL_FreeSurface(surf);
    SDL_DestroyTexture(text);

    return;
}

static void draw_keys(context_t *context)
{

    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {

            if (context->chip->keypad[4 * y + x]) {
                SDL_SetRenderDrawColor(context->ren, 255, 255, 0, 255);                
            } else {
                SDL_SetRenderDrawColor(context->ren, 100, 100, 100, 255);
            }

            SDL_RenderFillRect(context->ren, &(SDL_Rect){
                screen_size[0] / 2 + (key_size + key_gap) * (x - 2),
                (key_size + key_gap) * y + CHIPBOTTOM + (screen_size[1] - CHIPBOTTOM - KEYHEIGHT) / 2,
                key_size,
                key_size
            });

        }
    }

    return;
}

static void main_loop(context_t *context)
{
    double frame_start = NOW;

    ch8_cpu_inf_loop_fallback();
    Chip8Utils.ProcessCycles(context->chip, CCPF);

    SDL_SetRenderDrawColor(context->ren, 22, 22, 22, 255);
    SDL_RenderClear(context->ren);

    SDL_SetRenderDrawColor(context->ren, 0, 0, 0, 255);
    SDL_RenderFillRect(context->ren, &(SDL_Rect){
        CHIPLEFT,
        CHIPTOP,
        UNPACK2(chip_screen_size)
    });

    draw_chip(context);
    draw_edge(context);
    draw_keys(context);
    draw_filename(context);

    SDL_RenderPresent(context->ren);

    while (NOW - frame_start < 1 / FPS);

    return;
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        if (argc < 2) {
            printf("No ROM given.\n");
        }
        printf("Format must be \"./Chip8TYD 'path/to/your/rom.ch8'\\n\".\n");
        return 84;
    }

    TTF_Init();

    init_context(&context);
    context.filepath = argv[1];

    Chip8Utils.InitChip(&context.chip, wait_for_input, update_keys);
    Chip8Utils.LoadChip(context.chip, argv[1]);
    Chip8Utils.set_seed(time(NULL));

    //emscripten_set_main_loop_args(main_loop, &context, -1, 1);
    while (context.quit == 0) {
        main_loop(&context);
    }

    Chip8Utils.FreeChip(context.chip);
    SDL_DestroyRenderer(context.ren);
    SDL_DestroyWindow(context.win);
    TTF_CloseFont(context.font);
    SDL_Quit();


    return 0;
}
