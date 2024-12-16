//chip8.h

#ifndef CHIP8TYD
    #define CHIP8TYD

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

typedef struct Chip8Struct {

    unsigned char keypad[16];

    unsigned int frame_buffer[32*64];
    unsigned char ROM[4096];
    unsigned short int stack[16];

    unsigned char reg[16];
    unsigned char delay_timer;
    unsigned char sound_timer;
    unsigned short int index;
    unsigned short int program_counter;
    unsigned char stack_pointer;

    int (*wait_for_input)(void);
    void (*update_keys)(unsigned char (*keys)[16]);
    int has_drawn;

} Chip8;

typedef struct {
    void (*ExecInstruction)(Chip8 *chip);
    void (*InitChip)(Chip8 **chip, int (*wait_for_input)(void), void (*update_keys)(unsigned char (*keys)[16]));
    void (*LoadChip)(Chip8 *chip, char *filename);
    void (*ProcessFrame)(Chip8 *chip);
} chip8utils_t;

extern const chip8utils_t Chip8Utils;

#endif