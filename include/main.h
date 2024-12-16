//header.h

#ifndef MAINDEF
    #define MAINDEF

#include <SFML/Graphics/RenderWindow.h>
#include <SFML/Graphics/Sprite.h>
#include <SFML/Graphics/Texture.h>
#include <SFML/Window/Keyboard.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "chip8.h"
#include "framebuffer.h"

char *read_file(char *filename);

#endif
