//header.h

#ifndef MAINDEF
    #define MAINDEF

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "chip8.h"
#include "framebuffer.h"
#include "audio.h"
#include <SDL3/SDL.h>

#define UNPACK2(value) ((value)[0]), ((value)[1])

int read_file(char **kronk_buffer, char *filename);

#endif
