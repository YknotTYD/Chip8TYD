//chip8.c

//0x000 -> 0x200 => System (fonts)
//0x200 -> 0xfff => User program
//
//keypad[16] => 1234QWERASDFZXCV

//display the entire ROM
//prevent segfault when keypad[VX] && VX >= 16 and such
//make a chip-8 assembler
//fix FX65
//fix emulation speed
//add sound
//check 8XY6
//add pc to asm display
//make it a library and push it on PYPI
//add an emulation speed factor
//check for stuff that could segv
//investigate stars's abyss
//add padding to ASM repr?
//put chip->ROM and chip->frame_buffer on the heap?
//compile to WASM

#include "../include/chip8.h"

static int ExecInstruction(Chip8 *chip)
{

    unsigned short int opcode=(chip->ROM[chip->program_counter]<<8) |
                               chip->ROM[chip->program_counter+1];

    const int VX=(opcode&0x0F00)>>8;
    const int VY=(opcode&0x00F0)>>4;
    const int NNN=(opcode&0x0FFF);
    const int NN=(opcode&0x00FF);
    static const int VF=15;
    static int drw_index;

    char *asm_repr = bin_to_ASM(opcode);

    if (asm_repr) {
        printf("%s - 0x%x // -> 0x%04x\n",
            asm_repr, chip->program_counter, opcode);
        free(asm_repr);
    } else {
        printf("Unrecognized opcode: [? - 0x%x // -> 0x%04x].\n",
            chip->program_counter, opcode);
        return 1;
    }

    if ((NOW - chip->last_timer_update) >= 1 / 60.0) {
    
        chip->last_timer_update = NOW;

        if (chip->delay_timer > 0) {
            chip->delay_timer--;
        }
        if (chip->sound_timer > 0) {
            chip->sound_timer--;
        }
    }

    chip->has_drawn=0;

    switch (opcode&0xF000) {

        case 0X0000: //0x00E

            switch (opcode) {

                case 0x00E0: //0x00E0 - CLS
                    for (int i=0; i<32*64; i++) {
                        chip->frame_buffer[i]=0;
                    }
                    chip->has_drawn=1;
                    break;

                case 0x00EE: //0x00EE - RET
                    chip->stack_pointer--;
                    chip->program_counter = chip->stack[(int)(chip->stack_pointer)]-2; //-2 to compensate for the +2 at the end
                    break;
            }
            break;

        case 0x1000: //0x1NNN - JMP NNN
            chip->program_counter=NNN-2; //-2 to compensate for the +2 at the end
            break;

        case 0x2000: //0x2NNN - CALL NNN
            chip->stack[(int)(chip->stack_pointer)]=chip->program_counter+2;
            chip->program_counter=NNN-2; //-2 to compensate for the +2 at the end
            chip->stack_pointer++;
            break;

        case 0x3000: //0x3XNN - SE VX NN
            if (chip->reg[VX] == NN) {
                chip->program_counter+=2;
            }
            break;

        case 0x4000: //0x4XNN - SNE VX NN
            if (chip->reg[VX] != NN) {
                chip->program_counter+=2;
            }
            break;

        case 0x5000: //0x5XY0 - SE VX VY
            if (chip->reg[VX] == chip->reg[VY]) {
                chip->program_counter+=2;
            }
            break;

        case 0x6000: //0x6XNN - LD VX NN
            chip->reg[VX]=NN;
            break;

        case 0x7000: //0x7XNN - ADD VX NN
            chip->reg[VX]+=NN;
            break;

        case 0x8000: //0x8XYN

            switch (opcode&0X000F) {

                case 0x0000: //0x8XY0 - LD VX VY
                    chip->reg[VX]=chip->reg[VY];
                    break;

                case 0x0001: //0x8XY1 - OR VX VY
                    chip->reg[VX]=chip->reg[VX] | chip->reg[VY];
                    break;

                case 0x0002: //0x8XY2 - AND VX VY
                    chip->reg[VX]=chip->reg[VX] & chip->reg[VY];
                    break;

                case 0x0003: //0x8XY3 - XOR VX VY
                    chip->reg[VX]=chip->reg[VX] ^ chip->reg[VY];
                    break;

                case 0x0004: //0x8XY4 - ADD VX VY
                    chip->reg[VX]=chip->reg[VX] + chip->reg[VY];
                    chip->reg[VF]=0;
                    if (((int)chip->reg[VX] + (int)chip->reg[VY])>255) {
                        chip->reg[VF]=1;
                    }
                    break;

                case 0x0005: //0x8XY5 - SUB VX VY
                    chip->reg[VF]=(int)chip->reg[VX] > (int)chip->reg[VY];
                    chip->reg[VX]=chip->reg[VX] - chip->reg[VY];
                    break;

                case 0x0006: //0x8X(Ynot0?)6 - SHR Vx {,Vy}
                    chip->reg[VF]=chip->reg[VX]&1;
                    chip->reg[VX]=chip->reg[VX]>>1;
                    break;

                case 0x0007: //0x8XY7 - SUBN
                    chip->reg[VF]=(int)chip->reg[VX] < (int)chip->reg[VY];
                    chip->reg[VX]=chip->reg[VY]-chip->reg[VX];
                    break;

                case 0x0008: //0x8XY8 - SHL VX {, VY} (//DOESN'T EXIT?)
                    chip->reg[VF]=chip->reg[VX]&0x80;
                    chip->reg[VX]=chip->reg[VX]<<1;
                    break;

                case 0x0009: //0x8XY9 - SNE VX VY (//DOESN'T EXIT?)
                    if (chip->reg[VX]!=chip->reg[VY]) {
                        chip->program_counter++;
                    }
                    break;

                case 0x000E: //0x8X0E - SHFT VX (//DOESN'T EXIT?)
                    chip->reg[VF] = chip->reg[VX]>>7;
                    chip->reg[VX] = chip->reg[VX]<<1;
                    break;
            }
            break;

        case 0x9000: //0x9XY0 - SNE VX VY
            if (chip->reg[VX]!=chip->reg[VY]) {
                chip->program_counter+=2;
            }
            break;

        case 0xA000: //0xANNN - LD I NNN
            chip->index=NNN;
            break;

        case 0xB000: //0xBNNN - JUMP V0 NNN
            chip->program_counter=chip->reg[0]+NNN-2; //-2 to compensate for the +2 at the end
            break;

        case 0xC000: //0xCXNN - RAND VX NN
            chip->reg[VX]=rand()&NN;
            break;

        case 0xD000: //0xDXYN - DRW X Y N

            chip->reg[VF]=0;

            for (int y=0; y<(opcode&0x000F); y++){
                for (int x=0; x<8; x++) {

                    if (!((chip->ROM[chip->index+y]) & (1<<x))) {
                        continue;
                    }

                    drw_index=(7-x)+64*y+chip->reg[VX]+64*(chip->reg[VY]); //7-x to reverse the byte

                    if (drw_index<0 || drw_index>=32*64) {
                        continue;
                    }

                    if (chip->frame_buffer[drw_index]) {
                        chip->frame_buffer[drw_index]=0;
                        chip->reg[VF]=1;
                        continue;
                    }

                    chip->frame_buffer[drw_index]=1;

                }
            }
            chip->has_drawn=1;
            break;

        case 0xE000: //0xEX00

            switch (opcode&0x00F0) {

                case 0x0090: //0xEX9E - SKP VX
                    if (chip->keypad[(int)(chip->reg[VX])]) {
                        chip->program_counter+=2;
                    }
                    break;

                case 0x00A0: //0xEXA1 - SKNP VX
                    if (!(chip->keypad[(int)(chip->reg[VX])])) {
                        chip->program_counter+=2;
                    }
                    break;

            }
            break;

        case 0XF000: //0xFX00

            switch (opcode&0xF0FF) {

                case 0xF007: //0xFX07 - LD VX DT
                    chip->reg[VX]=chip->delay_timer;
                    break;

                case 0xF00A: //0xFX0A - LD VX K
                    chip->reg[VX]=(chip->wait_for_input)();
                    break;

                case 0xF015: //0xFX15 - LD DT VX
                    chip->delay_timer=chip->reg[VX];
                    break;

                case 0xF018: //0xFX18 - LD ST VX
                    chip->sound_timer=chip->reg[VX];
                    break;

                case 0xF01E: //0xFX1E - ADD I VX
                    chip->index+=chip->reg[VX];
                    break;

                case 0xF029: //0xFX29 - LD F VX
                    chip->index=chip->reg[VX]*5;
                    break;

                case 0xF033:
                    chip->ROM[chip->index+0]=chip->reg[VX]/100;
                    chip->ROM[chip->index+1]=(chip->reg[VX]/10)%10;
                    chip->ROM[chip->index+2]=chip->reg[VX]%10;
                    break;

                case 0xF055:
                    for (int i=0; i<=VX; i++) {
                        chip->ROM[chip->index]=chip->reg[i];
                        chip->index++;
                    }
                    break;

                case 0xF065: //0xFX65 - LD VX I //TODO: [FIX]
                    for (int i=0; i<=VX; i++) {
                        chip->reg[i]=chip->ROM[chip->index];
                        chip->index++;
                    }
                    break;

            }
            break;
    }
    chip->program_counter+=2;
    return 0;
}

static void InitChip(Chip8 **chip, int (*wait_for_input)(void), void (*update_keys)(unsigned char (*keys)[16]))
{

    (*chip) = malloc(sizeof(Chip8));

    (*chip)->wait_for_input = wait_for_input;
    (*chip)->update_keys = update_keys;

    for (int i = 0; i < 32 * 64; i++) {
        (*chip)->frame_buffer[i]=0;
    }
    for (int i=0; i<4096; i++) {
        (*chip)->ROM[i]=0;
    }
    for (int i=0; i<16; i++) {
        (*chip)->stack[i]=0;
        (*chip)->reg[i]=0;
        (*chip)->keypad[i]=0;
    }

    (*chip)->delay_timer=0;
    (*chip)->sound_timer=0;
    (*chip)->index=0;
    (*chip)->program_counter=0x200;
    (*chip)->stack_pointer=0;
    (*chip)->last_timer_update=0;

    return;
}

static void FreeChip(Chip8 *chip)
{
    free(chip);
    return;
}

static void ProcessFrame(Chip8 *chip, int frame_number, int (*fallback_function)(void *args), void *args) {

    chip->update_keys(&(chip->keypad));
    for (int i = 0; i < frame_number; i++) {
        Chip8Utils.ExecInstruction(chip);
    }

    return;

    do {
    chip->update_keys(&(chip->keypad));
        ExecInstruction(chip);
    } while (chip->has_drawn == 0 && fallback_function(args));

    return;
}

static int read_file(char **kronk_buffer, char *filepath)
{
    int fildes;
    int lenght;
    int readsult;

    fildes = open(filepath, O_RDONLY);
    if (fildes == -1) {
        return 0;
    }
    lenght = lseek(fildes, 0, SEEK_END);
    lseek(fildes, 0, SEEK_SET);
    *kronk_buffer = malloc(lenght + 1);
    if (kronk_buffer == 0) {
        close(fildes);
        return 0;
    }
    readsult = read(fildes, *kronk_buffer, lenght);
    if (readsult == 0) {
        free(*kronk_buffer);
    }
    (*kronk_buffer)[lenght] = 0;
    close(fildes);
    return lenght;
}


static void LoadChip(Chip8 *chip, char *filename)
{
    static const unsigned char fontset[] = {
    	0xF0, 0x90, 0x90, 0x90, 0xF0, //0
    	0x20, 0x60, 0x20, 0x20, 0x70, //1
    	0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
    	0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
    	0x90, 0x90, 0xF0, 0x10, 0x10, //4
    	0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
    	0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
    	0xF0, 0x10, 0x20, 0x40, 0x40, //7
    	0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
    	0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
    	0xF0, 0x90, 0xF0, 0x90, 0x90, //A
    	0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
    	0xF0, 0x80, 0x80, 0x80, 0xF0, //C
    	0xE0, 0x90, 0x90, 0x90, 0xE0, //D
    	0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
    	0xF0, 0x80, 0xF0, 0x80, 0x80, //F
        0x00
    };
    char *ops;
    int size = read_file(&ops, filename);

    for (int i = 0; fontset[i]; i++) {
        chip->ROM[i] = fontset[i];
    }
    for (int i = 0; i < size; i ++) {
        chip->ROM[0x200 + i] = ops[i];
    }
    return;
}

static void set_seed(long long int seed)
{
    srand(seed);
    return;
}

const chip8utils_t Chip8Utils = {
    ExecInstruction, InitChip,
    FreeChip, LoadChip,
    ProcessFrame, set_seed
};
