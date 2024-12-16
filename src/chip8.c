//chip8.c

//0x000 -> 0x200 => System (fonts)
//0x200 -> 0xfff => User program
//
//reg[0] -> reg[15] => V0 -> VF
//VF => carry/borrow flag (shouldn't be accessed directly)
//
//DT => -=1 at 60HZ when > 0
//ST => same as DT but buzzes when > 0 (can't be read)
//
//keypad[16] => 1234QWERASDFZXCV
//
//8x15 sprites

//Check for VF in 0X7XNN
//Give variables for VX and VY instead of doing the huge thing each and every time
//Same for NN and NNN?
//
//display instructions as opcode - SUBN VX VY
//display the entire ROM
//Valgroind everything
//prevent segfault when keypad[VX] && VX >= 16 and such
//make a chip-8 assembler

#include "../include/chip8.h"

static void ExecInstruction(Chip8 *chip)
{

    short int opcode=(chip->ROM[chip->program_counter]<<8) |
                      chip->ROM[chip->program_counter+1];

    const int VX=(opcode&0x0F00)>>8;
    const int VY=(opcode&0x00F0)>>4;
    const int VF=15;
    static int drw_index;

    chip->has_drawn=0;

    //TODO: 60hz dt and st decrease

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
            chip->program_counter=(opcode&0x0FFF)-2; //-2 to compensate for the +2 at the end
            break;

        case 0x2000: //0x2NNN - CALL NNN
            chip->stack[(int)(chip->stack_pointer)]=chip->program_counter+2;
            chip->program_counter=(opcode&0x0FFF)-2; //-2 to compensate for the +2 at the end
            chip->stack_pointer++;
            break;

        case 0x3000: //0x3XNN - SE VX NN
            if (chip->reg[VX] == (opcode&0X00FF)) {
                chip->program_counter+=2;
            }
            break;

        case 0x4000: //0x4XNN - SNE VX NN
            if (chip->reg[VX] != (opcode&0X00FF)) {
                chip->program_counter+=2;
            }
            break;

        case 0x5000: //0x5XY0 - SE VX VY
            if (chip->reg[VX] == chip->reg[VY]) {
                chip->program_counter+=2;
            }
            break;

        case 0x6000: //0x6XNN - LD VX NN
            chip->reg[VX]=opcode&0x00FF;
            break;

        case 0x7000: //0x7XNN - ADD VX NN
            chip->reg[VX]+=opcode&0x00FF;
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
                    chip->reg[VX]=chip->reg[VX] - chip->reg[VY];
                    chip->reg[VF]=0;
                    if ((int)chip->reg[VX] > (int)chip->reg[VY]) {
                        chip->reg[VF]=1;
                    }
                    break;

                case 0x0006: //0x8X(Ynot0?)6 - SHR Vx {,Vy}
                    chip->reg[VF]=chip->reg[VX]&1;
                    chip->reg[VX]=chip->reg[VX]>>1;
                    break;

                case 0x0007: //0x8XY7 - SUBN
                    chip->reg[VF]=(chip->reg[VX]>chip->reg[VY]);
                    chip->reg[VX]=chip->reg[VY]-chip->reg[VX];
                    break;

                case 0x0008: //0x8XY8 - SHL VX {, VY}
                    chip->reg[VF]=chip->reg[VX]&0x80;
                    chip->reg[VX]=chip->reg[VX]<<1;
                    break;

                case 0x0009: //0x8XY9 - SNE VX VY
                    if (chip->reg[VX]!=chip->reg[VY]) {
                        chip->program_counter++;
                    }
                    break;
            }
            break;

        case 0x9000: //0x9XY0 - SNE VX VY
            if (chip->reg[VX]!=chip->reg[VY]) {
                chip->program_counter+=2;
            }
            break;

        case 0xA000: //0xANNN - LD I NNN
            chip->index=opcode&0x0FFF;
            break;

        case 0xB000: //0xBNNN - JUMP V0 NNN
            chip->program_counter=chip->reg[0]+(opcode&0x0FFF);
            break;

        case 0xC000: //0xCXNN - RAND VX NN
            chip->reg[VX]=rand()&(opcode&0x00FF); //TODO: check if this works
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

                case 0xF033: //0xFX33 - LD B VX
                    chip->ROM[chip->index+0]=chip->reg[VX]/100;
                    chip->ROM[chip->index+1]=(chip->reg[VX]/10)%10;
                    chip->ROM[chip->index+2]=chip->reg[VX]%10;
                    break;

                case 0xF055: //0xFX55 - LD VX I
                    for (int vi=0; vi<VX; vi++) {
                        chip->reg[vi]=chip->ROM[chip->index+vi];
                    }
                    break;
                
            }
            break;
    }
    chip->program_counter+=2;
    return;
}

static void InitChip(Chip8 **chip, int (*wait_for_input)(void), void (*update_keys)(unsigned char (*keys)[16]))
{

    (*chip) = malloc(sizeof(Chip8));

    (*chip)->wait_for_input=wait_for_input;
    (*chip)->update_keys=update_keys;

    for (int i=0; i<32*64; i++) {
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

    return;
}

static void ProcessFrame(Chip8 *chip) {
    do {
        chip->update_keys(&(chip->keypad));
        ExecInstruction(chip);
    } while (chip->has_drawn == 0);
    return;
}

static void LoadChip(Chip8 *chip, char *filename)
{

    chip->ROM[0x500]=0b10000000;
    chip->ROM[0x501]=0b10000000;
    chip->ROM[0x502]=0b10000000;
    chip->ROM[0x503]=0b10000000;
    chip->ROM[0x504]=0b10000000;
    chip->ROM[0x505]=0b10000000;
    chip->ROM[0x506]=0b10000000;
    chip->ROM[0x507]=0b10000000;

    chip->ROM[0x508]=0b10000000;

    //0xEXA1 - SKP
    //0x8XY5 - SUB VX VY
    //FX33 - LD B VX (1 2 3 in ROM)

    //reg0 -> x
    //reg1 -> y
    //reg3 -> s index
    //reg4 -> 1
    //reg5 -> z index
    //reg6 -> ballx
    //reg7 -> bally
    //reg8 -> ballxvel+
    //reg9 -> ballyvel+
    //regA -> ballxvel-
    //regB -> ballyvel-
    //regC -> pong2 x

    short int ops[] = {
        0x6308, //0x200 - put keypad s index in reg3
        0x6401, //0x202 - put 1 in reg4 (to sub from reg1)
        0x650A, //0x204 - put keypad z index in reg5
        0x6620, //0x204 - set reg6 to 32
        0x6710, //0x208 - set reg7 to 16
        0x6801, //0x20A - set reg8 to 1
        0x6900, //0x20C - set reg9 to 0
        0x6A00, //0x20E - set regA to 0
        0x6B01, //0x210 - set regB to 1
        0x6C3F, //0x212 - set regC to 63
                //
        0x2258, //0x214 - call draw func
                //
        0xE3A1, //0x216 - skip if reg3 key (s) is pressed
        0x7101, //0x218 - y++; (if reg3 key (s) is pressed)
        0xE5A1, //0x21A - skip if reg5 key (z) is pressed
        0x8145, //0x21C - y -= reg4 (1) (if reg5 key (z) is pressed)
                //
        0x4119, //0x21E - skip if reg1 != 32
        0x6118, //0x220 - y = (32 - 8); (if y == 32)
        0x41ff, //0x222 - skip if reg1 != 255
        0x6100, //0x224 - y = 0; (if y == 255)
                //
        0x371F, //0x226 - skip if reg7 == 31
        0x122E, //0x228 - jump to x upper bounce check (if no upper y bounce)
        0x6900, //0x22A - set reg9  to 0 (0 pos yvel)
        0x6B01, //0x22C - set regB to 1 (1 neg yvel) 
                //
        0x363E, //0x22E - skip if reg6 == 62
        0x1236, //0x230 - jump to y lower bounce check (if no upper x bounce)
        0x6800, //0x232 - set reg8  to 0 (0 pos xvel)
        0x6A01, //0x234 - set regA to 1 (1 neg xvel)
                //
        0x3700, //0x236 - skip if reg7 == 0
        0x1240, //0x238 - jump to lower x bounce check (if no lower y bounce)
        0x6901, //0x23A - set reg9  to 1 (1 pos yvel)
        0x6B00, //0x23C - set regB to 0 (0 neg yvel)
        0x6700, //0x23E - set reg7 to 0 (0 bally)
                //
        0x3601, //0x240 - skip if reg6 == 1
        0x124C, //0x242 - jump to reg[6-7] update (if no lower x bounce)
        0x6801, //0x244 - set reg8  to 0 (1 pos xvel)
        0x6A00, //0x246 - set regA to 0 (0 neg xvel)
        0x6601, //0x248 - set reg6 to 1 (1 ballx)
        0x2256, //0x24A - call bar collision check
                //
        0x8684, //0x24C - reg6 += reg8
        0x8794, //0x24E - reg7 += reg9
        0x86A5, //0x250 - reg6 -= regA
        0x87B5, //0x252 - reg7 -= regB
                //
        0x1214, //0x254 - loop to clear
                //
                //bar collision check
                //
        //0x7000, //  
        0x00EE, //0x256 - return from call
                //
                //draw
                //
        0x00E0, //0x258 - clear
        0xA508, //0x25A - set index to 0x508 (ball sprite)
        0xD671, //0x25C - draw reg6 reg7 (ball sprite)
        0xA500, //0x25E - set index to 0x500 (pong sprite)
        0xD018, //0x260 - draw reg0 reg1 (pong sprite)
        0xDC78, //0x262 - draw regC reg7 (pong2 sprite)
        0x00EE, //0x26E - return
                //
        0x0000, //0x270
    };

    for (int i=0; ops[i]; i++) {
        chip->ROM[0x200+i*2]=ops[i]>>8;
        chip->ROM[0x200+i*2+1]=ops[i]&0x00FF;
    }

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

    for (int i = 0; fontset[i]; i++) {
        chip->ROM[i] = fontset[i];
    }
    printf("Unsuccessfully loaded file '%s'.\n", filename);
    return;
}

const chip8utils_t Chip8Utils = {
    ExecInstruction, InitChip,
    LoadChip, ProcessFrame
};
