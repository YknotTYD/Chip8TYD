//main.c

#include "../include/main.h"

//set framerate
//switch to SDL3

static const int vlr=22;
static const int screen_size[2]={64*vlr, 32*vlr};
static const unsigned char keys[16]={
    sfKeyX, sfKeyNum1, sfKeyNum2, sfKeyNum3,
    sfKeyQ, sfKeyW, sfKeyE, sfKeyA, sfKeyS,
    sfKeyD, sfKeyZ, sfKeyC, sfKeyNum4, sfKeyR,
    sfKeyF, sfKeyV
};

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
}

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

int main()
{

    static Chip8 *chip;

    Chip8Utils.InitChip(&chip, wait_for_input, update_keys);
    Chip8Utils.LoadChip(chip, "files/roms/tetris.ch8");

    Chip8Utils.set_seed(time(NULL));

    sfVideoMode mode = {screen_size[0], screen_size[1], 32};
    sfRenderWindow* window;
    sfTexture* texture;
    sfSprite* sprite; 
    FrameBuffer *fbuffer;
    sfEvent event;

    window = sfRenderWindow_create(mode, "Chip8TYD", sfResize | sfClose, NULL);
    fbuffer = new_frame_buffer(screen_size[0], screen_size[1]);
    texture = sfTexture_create(screen_size[0], screen_size[1]);
    sprite = sfSprite_create();

    sfRenderWindow_setFramerateLimit(window, 120);
    clear_buffer(fbuffer);

    while (sfRenderWindow_isOpen(window)) {

        while (!sfRenderWindow_hasFocus(window));

        f4_alt((void *[2]){window, &event});

        Chip8Utils.ProcessFrame(chip, f4_alt, (void *[2]){window, &event});
        draw_chip(chip, fbuffer, vlr);

        sfTexture_updateFromPixels(
            texture, fbuffer->pixels,
            screen_size[0], screen_size[1],
            0, 0
        );
        sfSprite_setTexture(sprite, texture, sfFalse);
        sfRenderWindow_clear(window, sfBlack);
        clear_buffer(fbuffer);

        sfRenderWindow_drawSprite(window, sprite, NULL);
        sfRenderWindow_display(window);
    }

    sfSprite_destroy(sprite);
    sfTexture_destroy(texture);
    sfRenderWindow_destroy(window);

    Chip8Utils.FreeChip(chip);

    return 0;
}
