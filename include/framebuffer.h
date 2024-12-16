//framebuffer.h

#ifndef FRAMEBUFFERDEF
    #define FRAMEBUFFERDEF

#include <stdlib.h>
#include <string.h>

typedef struct FrameBufferStruct {
unsigned char *pixels;
    unsigned int width;
    unsigned int heigth;
    unsigned int size[2];
} FrameBuffer;

FrameBuffer *new_frame_buffer(int width, int heigth);
void draw_square(FrameBuffer *fbuffer, int xpos, int ypos,
    int width, int heigth, char color[4]);
void clear_buffer(FrameBuffer *fbuffer);

#endif
