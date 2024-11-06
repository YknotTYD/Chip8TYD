//main.c

#include <SFML/Graphics/RenderWindow.h>
#include <SFML/Graphics/Sprite.h>
#include <SFML/Graphics/Texture.h>
#include <SFML/Window/Keyboard.h>
//#include <SFML/System/Time.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "chip8.h"

//draw fbuff
//put fbuff in text
//link text to sprite
//draw sprite on window
//flip

static unsigned char keys[16]={sfKeyX, sfKeyNum1, sfKeyNum2, sfKeyNum3,
                                sfKeyQ, sfKeyW, sfKeyE, sfKeyA, sfKeyS,
                                sfKeyD, sfKeyZ, sfKeyC, sfKeyNum4, sfKeyR,
                                sfKeyF, sfKeyV};

typedef struct FrameBufferStruct {
unsigned char *pixels;
    unsigned int width;
    unsigned int heigth;
    unsigned int size[2];
} FrameBuffer;

FrameBuffer *new_frame_buffer(int width, int heigth)
{
    FrameBuffer *fbuffer = malloc(sizeof(FrameBuffer));

    fbuffer->pixels = malloc(32*width*heigth);
    fbuffer->width = width;
    fbuffer->heigth = heigth;

    fbuffer->size[0] = width;
    fbuffer->size[1] = heigth;

    return fbuffer;
}

void draw_square(FrameBuffer *fbuffer, int xpos, int ypos,
                 int width, int heigth, char color[4])
{
    for (int y = ypos; y < heigth + ypos; y++){
        for (int x = xpos; x < width + xpos; x++){
        
            if (x < 0  || x >= (int)(fbuffer->width) ||
                y >= (int)(fbuffer->heigth) || y < 0) {
                continue;
            }

            for (int i = 0; i < 4; i++) {
                (fbuffer->pixels)[(x+y*(fbuffer->width))*4+i]=color[i];
            }
        }
    }
}

void clear_buffer(FrameBuffer *fbuffer)
{
    memset((fbuffer->pixels), 55, (fbuffer->width*fbuffer->heigth*32));
}

int wait_for_input()
{
    while (1) {
        for (int i=0; i<16; i++) {
            if (sfKeyboard_isKeyPressed(keys[i])) {
                return i;
            }
        }
    }
}

void update_keys(unsigned char (*keypad)[16])
{
    for (int i=0; i<16; i++) {
        (*keypad)[i]=sfKeyboard_isKeyPressed(keys[i]);
    }
 }

int main()
{
    const int vlr=22;
    const int screen_size[2]={64*vlr, 32*vlr};

    Chip8 chip;

    InitChip(&chip, &wait_for_input, &update_keys);
    LoadChip(&chip, "logo.ch8");

    srand(time(NULL));
    
    sfVideoMode mode = {screen_size[0], screen_size[1], 32};
    sfRenderWindow* window;
    sfTexture* texture;
    sfSprite* sprite; 
    FrameBuffer *fbuffer;
    sfEvent event;

    window = sfRenderWindow_create(mode, "Chip 8 Emulator", sfResize | sfClose, NULL);
    fbuffer = new_frame_buffer(screen_size[0], screen_size[1]);
    texture = sfTexture_create(screen_size[0], screen_size[1]);
    sprite = sfSprite_create();

    while (sfRenderWindow_isOpen(window))
    {
        //while (!sfRenderWindow_hasFocus(window));

        do {
            chip.update_keys(&(chip.keypad));
            ExecInstruction(&chip);
        } while (chip.has_drawn == 0);

        while (sfRenderWindow_pollEvent(window, &event))
        {
            if (event.type == sfEvtClosed)
                sfRenderWindow_close(window);
        }

        sfTexture_updateFromPixels(texture, fbuffer->pixels,
                                   screen_size[0], screen_size[1],
                                   0, 0);
        sfSprite_setTexture(sprite, texture, sfFalse);

        sfRenderWindow_clear(window, sfBlack);
        clear_buffer(fbuffer);

        for (int y=0; y<32; y++) {
            for (int x=0; x<64; x++) {
                if (!chip.frame_buffer[y*64+x]) {
                    continue;
                }
                draw_square(fbuffer, x*vlr, y*vlr, vlr, vlr,
                            (char[4]){20, 200, 32, 255});
            }
        }

        sfRenderWindow_drawSprite(window, sprite, NULL);
        sfRenderWindow_display(window);
        if (chip.keypad[0]) {
            sleep(1);
        }
    }

    sfSprite_destroy(sprite);
    sfTexture_destroy(texture);
    sfRenderWindow_destroy(window);

    return 0;
}
