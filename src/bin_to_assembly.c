//bin_to_assembly.c

char *bin_to_ASM(unsigned short int opcode)
{
    /*const int VX=(opcode&0x0F00)>>8;
    const int VY=(opcode&0x00F0)>>4;
    const int NNN=(opcode&0x0FFF);
    const int NN=(opcode&0x00FF);
    static const int VF=15;
    static int drw_index;*/

    switch (opcode&0xF000) {

        case 0X0000: //0x00E

            switch (opcode) {

                case 0x00E0: //0x00E0 - CLS
                    return "CLS";

                case 0x00EE: //0x00EE - RET
                    return "RET";
            }

            break;

        case 0x1000: //0x1NNN - JMP NNN
            return "JMP NNN";

        case 0x2000: //0x2NNN - CALL NNN
            return "CALL NNN";

        case 0x3000: //0x3XNN - SE VX NN
            return "SE VX NN";

        case 0x4000: //0x4XNN - SNE VX NN
            return "SNE VX NN";

        case 0x5000: //0x5XY0 - SE VX VY
            return "SE VX VY";

        case 0x6000: //0x6XNN - LD VX NN
            return "LD VX NN";

        case 0x7000: //0x7XNN - ADD VX NN
            return "ADD VX NN";

        case 0x8000: //0x8XYN

            switch (opcode&0X000F) {

                case 0x0000: //0x8XY0 - LD VX VY
                    return "LD VX VY";

                case 0x0001: //0x8XY1 - OR VX VY
                    return "OR VX VY";

                case 0x0002: //0x8XY2 - AND VX VY
                    return "AND VX VY";

                case 0x0003: //0x8XY3 - XOR VX VY
                    return "XOR VX VY";

                case 0x0004: //0x8XY4 - ADD VX VY
                    return "ADD VX VY";

                case 0x0005: //0x8XY5 - SUB VX VY
                    return "SUB VX VY";

                case 0x0006: //0x8X(Ynot0?)6 - SHR Vx {,Vy}
                    return "SHR Vx {,Vy}";

                case 0x0007: //0x8XY7 - SUBN
                    return "SUBN VX VY";

                case 0x0008: //0x8XY8 - SHL VX {, VY} (//DOESN'T EXIT?)
                    return "SHL VX {, VY}";

                case 0x0009: //0x8XY9 - SNE VX VY (//DOESN'T EXIT?)
                    return "SNE VX VY";

                case 0x000E: //0x8X0E - SHFT VX (//DOESN'T EXIT?)
                    return "SHFT VX";
            }

            break;

        case 0x9000: //0x9XY0 - SNE VX VY
            return "SNE VX VY";

        case 0xA000: //0xANNN - LD I NNN
            return "LD I NNN";

        case 0xB000: //0xBNNN - JUMP V0 NNN
            return "JUMP V0 NNN";

        case 0xC000: //0xCXNN - RAND VX NN
            return "RAND VX NN";

        case 0xD000: //0xDXYN - DRW X Y N
            return "DRW X Y N";

        case 0xE000: //0xEX00

            switch (opcode&0x00F0) {

                case 0x0090: //0xEX9E - SKP VX
                    return "SKP VX";

                case 0x00A0: //0xEXA1 - SKNP VX
                    return "SKNP VX";

            }

            break;

        case 0XF000: //0xFX00

            switch (opcode&0xF0FF) {

                case 0xF007: //0xFX07 - LD VX DT
                    return "LD VX DT";

                case 0xF00A: //0xFX0A - LD VX K
                    return "LD VX K";

                case 0xF015: //0xFX15 - LD DT VX
                    return "LD DT VX";

                case 0xF018: //0xFX18 - LD ST VX
                    return "LD ST VX";

                case 0xF01E: //0xFX1E - ADD I VX
                    return "ADD I VX";

                case 0xF029: //0xFX29 - LD F VX
                    return "LD F VX";

                case 0xF033: //0xFX33 - LD B VX
                    return "LD B VX";

                case 0xF055: //0xFX55 - LD I VX
                    return "LD I VX";

                case 0xF065: //0xFX65 - LD VX I
                    return  "LD VX I";
                
            }
            break;
    }
    return 0;
}
