//bin_to_assembly.c

#include "../include/chip8.h"

static char *BTA_str_cat(char *str0, char *str1,
    char *str2, char *str3) {

    int l0;
    int l1;
    int l2;
    int l3;

    for (l0 = 0; str0[l0]; l0++);
    for (l1 = 0; str1[l1]; l1++);
    for (l2 = 0; str2 && str2[l2]; l2++);
    for (l3 = 0; str3 && str3[l3]; l3++);

    char *ret = malloc(l0 + l1 + l2 + l3 + 5);
    for (int i = 0; i < 5; i++) {
        ret[l0 + l1 + l2 + l3 + i] = '\0';
    }

    for (int i = 0; i < l0; i++) {
        ret[i] = str0[i];
    }
    ret[l0] = ' ';
    for (int i = 0; i < l1; i++) {
        ret[i + l0 + 1] = str1[i];
    }
    if (str2) {
        ret[l0 + l1 + 1] = ' ';
    }
    for (int i = 0; l2 && i < l2; i++) {
        ret[i + l0 + l1 + 2] = str2[i];
    }
    if (str3) {
        ret[l0 + l1 + l2 + 2] = ' ';
    }
    for (int i = 0; l3 && i < l3; i++) {
        ret[i + l0 + l1 + l2 + 3] = str3[i];
    }

    return ret;
}

static char *BTA_nbr_to_str(unsigned long long int nbr)
{
    char *str;
    int lenght = nbr == 0;
    int i;

    for (int cpy = nbr; cpy; cpy /= 16) {
        lenght++;
    }

    str = malloc(lenght + 1);
    str[lenght] = '\0';

    for (i = lenght - 1; i >= 0; i--) {
        str[i] = nbr % 10;
        nbr /= 10;
    }

    return str;
}

static char *BTA_nbr_to_str_hex(unsigned long long int nbr)
{
    char *str;
    int lenght = nbr == 0;
    int i;

    for (int cpy = nbr; cpy; cpy /= 16) {
        lenght++;
    }

    str = malloc(lenght + 1);
    str[lenght] = '\0';

    for (i = lenght - 1; i >= 0; i--) {
        str[i] = HEX(nbr % 16);
        nbr /= 16;
    }

    return str;
}

static char *add_v(char *reg)
{
    char *str;
    int lenght;

    for (lenght = 0; reg[lenght]; lenght++);
    str = malloc(lenght + 2);
    str[lenght + 1] = '\0';
    str[0] = 'V';
    for (int i = 0; i < lenght; i++) {
        str[i + 1] = reg[i];
    }
    free(reg);
    return str;
}

char *bin_to_ASM(unsigned short int opcode)
{
    char *ASM = 0;
    char *VX=add_v(BTA_nbr_to_str_hex((opcode&0x0F00)>>8));
    char *VY=add_v(BTA_nbr_to_str_hex((opcode&0x00F0)>>4));
    char *NNNX=BTA_nbr_to_str_hex((opcode&0x0FFF));
    char *NN=BTA_nbr_to_str((opcode&0x00FF));
    char *N=BTA_nbr_to_str((opcode&0x000F));
    char *VF=malloc(2);
    VF[0] = 'F';
    VF[1] = '\0';
    VF = add_v(VF);

    switch (opcode&0xF000) {

        case 0X0000: //0x00E

            switch (opcode) {

                case 0x00E0: //0x00E0 - CLS
                        ASM = "CLS";
                        break;

                case 0x00EE: //0x00EE - RET
                    ASM = "RET";
            }

            break;

        case 0x1000: //0x1NNN - JMP NNN
            ASM = BTA_str_cat("JMP", NNNX, 0, 0);
            break;

        case 0x2000: //0x2NNN - CALL NNN
            ASM = BTA_str_cat("CALL", NNNX, 0, 0);
            break;

        case 0x3000: //0x3XNN - SE VX NN
            ASM = BTA_str_cat("SE",  VX, NN, 0);
            break;

        case 0x4000: //0x4XNN - SNE VX NN
            ASM = BTA_str_cat("SNE", VX, NN, 0);
            break;

        case 0x5000: //0x5XY0 - SE VX VY
            ASM = BTA_str_cat("SE", VX, VY, 0);
            break;

        case 0x6000: //0x6XNN - LD VX NN
            ASM = BTA_str_cat("LD", VX, NN, 0);
            break;

        case 0x7000: //0x7XNN - ADD VX NN
            ASM = BTA_str_cat("ADD", VX, NN, 0);
            break;

        case 0x8000: //0x8XYN

            switch (opcode&0X000F) {

                case 0x0000: //0x8XY0 - LD VX VY
                    ASM = BTA_str_cat("LD", VX, VY, 0);
                    break;

                case 0x0001: //0x8XY1 - OR VX VY
                    ASM = BTA_str_cat("OR", VX, VY, 0); 
                    break;

                case 0x0002: //0x8XY2 - AND VX VY
                    ASM = BTA_str_cat("AND", VX, VY, 0);
                    break;

                case 0x0003: //0x8XY3 - XOR VX VY
                    ASM = BTA_str_cat("XOR", VX, VY, 0);
                    break;

                case 0x0004: //0x8XY4 - ADD VX VY
                    ASM = BTA_str_cat("ADD", VX, VY, 0);
                    break;

                case 0x0005: //0x8XY5 - SUB VX VY
                    ASM = BTA_str_cat("SUB", VX, VY, 0);
                    break;

                case 0x0006: //0x8X(Ynot0?)6 - SHR Vx {,Vy}
                    ASM = BTA_str_cat("SHR", VX, VY, 0);
                    break;

                case 0x0007: //0x8XY7 - SUBN
                    ASM = BTA_str_cat("SUBN", VX, VY, 0);
                    break;

                case 0x0008: //0x8XY8 - SHL VX {, VY} (//DOESN'T EXIT?)
                    ASM = BTA_str_cat("SHL", VX, VY, 0);
                    break;

                case 0x0009: //0x8XY9 - SNE VX VY (//DOESN'T EXIT?)
                    ASM = BTA_str_cat("SNE", VX, VY, 0);
                    break;

                case 0x000E: //0x8X0E - SHFT VX (//DOESN'T EXIT?)
                    ASM = BTA_str_cat("SHFT", VX, 0, 0);
                    break;
            }

            break;

        case 0x9000: //0x9XY0 - SNE VX VY
            ASM = BTA_str_cat("SNE", VX, VY, 0);
            break;

        case 0xA000: //0xANNN - LD I NNN
            ASM = BTA_str_cat("LD I", NNNX, 0, 0);
            break;

        case 0xB000: //0xBNNN - JUMP V0 NNN
            ASM = BTA_str_cat("JUMP V0", NNNX, 0, 0);
            break;

        case 0xC000: //0xCXNN - RAND VX NN
            ASM = BTA_str_cat("RAND", VX, NN, 0);
            break;

        case 0xD000: //0xDXYN - DRW X Y N
            ASM = BTA_str_cat("DRW", VX, VY, N);
            break;

        case 0xE000: //0xEX00

            switch (opcode&0x00F0) {

                case 0x0090: //0xEX9E - SKP VX
                    ASM = BTA_str_cat("SKP", VX, 0, 0);
                    break;

                case 0x00A0: //0xEXA1 - SKNP VX
                    ASM = BTA_str_cat("SKNP", VX, 0, 0);
                    break;

            }

            break;

        case 0XF000: //0xFX00

            switch (opcode&0xF0FF) {

                case 0xF007: //0xFX07 - LD VX DT
                    ASM = BTA_str_cat("LD", VX, "DT", 0);
                    break;

                case 0xF00A: //0xFX0A - LD VX K
                    ASM = BTA_str_cat("LD", VX, "K", 0);
                    break;

                case 0xF015: //0xFX15 - LD DT VX
                    ASM = BTA_str_cat("LD DT", VX, 0, 0);
                    break;

                case 0xF018: //0xFX18 - LD ST VX
                    ASM = BTA_str_cat("LD ST", VX, 0, 0);
                    break;

                case 0xF01E: //0xFX1E - ADD I VX
                    ASM = BTA_str_cat("ADD I", VX, 0, 0);
                    break;

                case 0xF029: //0xFX29 - LD F VX
                    ASM = BTA_str_cat("LD F", VX, 0, 0);
                    break;

                case 0xF033: //0xFX33 - LD B VX
                    ASM = BTA_str_cat("LD B", VX, 0, 0);
                    break;

                case 0xF055: //0xFX55 - LD I VX
                    ASM = BTA_str_cat("LD I", VX, 0, 0);
                    break;

                case 0xF065: //0xFX65 - LD VX I
                    ASM = BTA_str_cat( "LD", VX, "I", 0);
                    break;
                
            }
            break;
    }
    free(VX);
    free(VY);
    free(VF);
    free(N);
    free(NN);
    free(NNNX);
    return ASM;
}
