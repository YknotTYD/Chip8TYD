//framebuffer.h

#include "../include/framebuffer.h"

FrameBuffer *new_frame_buffer(int width, int heigth)
{
    FrameBuffer *fbuffer = malloc(sizeof(FrameBuffer));

    fbuffer->pixels = malloc(sizeof(int) * width * heigth);
    fbuffer->width = width;
    fbuffer->height = heigth;

    fbuffer->size[0] = width;
    fbuffer->size[1] = heigth;

    return fbuffer;
}

void draw_square(FrameBuffer *fbuffer, int xpos, int ypos,
    int width, int height, int color)
{
    for (int y = ypos; y < height + ypos; y++){
        for (int x = xpos; x < width + xpos; x++){
        
            if (x < 0  || x >= (int)(fbuffer->width) ||
                y >= (int)(fbuffer->height) || y < 0) {
                continue;
            }
            (fbuffer->pixels)[y * (fbuffer->width) + x] = color;
        }
    }
}

void clear_buffer(FrameBuffer *fbuffer)
{
    for (unsigned int i = 0; i < fbuffer->width * fbuffer->height; i++) {
        fbuffer->pixels[i] = 0x000000FF;
    }
    return;
}
