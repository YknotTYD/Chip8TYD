//chip8.h

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <time.h>

#ifndef CHIP8TYD

    #define CHIP8TYD
    #define NOW (clock()/(double)CLOCKS_PER_SEC)
    #define HEX(c) ((c) > 9 ? 'A' + (c) - 10 : (c) + '0')

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
    double last_timer_update;

} Chip8;

typedef struct {
    void (*ExecInstruction)(Chip8 *chip);
    void (*InitChip)(Chip8 **chip, int (*wait_for_input)(void), void (*update_keys)(unsigned char (*keys)[16]));
    void (*LoadChip)(Chip8 *chip, char *filename);
    void (*ProcessFrame)(Chip8 *chip, int (*fallback_function)(void *args), void *args);
    void (*set_seed)(long long int seed);
} chip8utils_t;

extern const chip8utils_t Chip8Utils;
char *bin_to_ASM(unsigned short int);

#endif
