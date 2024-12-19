//framebuffer.h

#ifndef FRAMEBUFFERDEF
    #define FRAMEBUFFERDEF

#include <stdlib.h>
#include <string.h>

typedef struct FrameBufferStruct {
    unsigned int *pixels;
    unsigned int width;
    unsigned int height;
    unsigned int size[2];
} FrameBuffer;

FrameBuffer *new_frame_buffer(int width, int heigth);
void draw_square(FrameBuffer *fbuffer, int xpos, int ypos,
    int width, int heigth, int color);
void clear_buffer(FrameBuffer *fbuffer);

#endif
