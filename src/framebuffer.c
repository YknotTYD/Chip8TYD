//framebuffer.h

#include "../include/framebuffer.h"

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
