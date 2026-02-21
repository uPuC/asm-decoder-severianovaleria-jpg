#include <stdio.h>
#include <inttypes.h>

const uint8_t flash_mem[] ={ //arreglo simula la memoria
    0x00, 0x24, //CLR 2400  0010 0100 0000 0000
    0xA0, 0xE0, //LDI E0A0 1110 0000 1010 0000
    0xB2, 0xE0, //LDI E0B2 1110 0000 1011 0010
    0x0D, 0x91, //LD 910D (POST INCREMENTED USING X) 0101 0001 0000 1101
    0x00, 0x30, //CPI 3000 0011 0000 0000 0000
    0xE9, 0xF7, //BRNE F7E9 1111 0111 1110 1001
    0x11, 0x97, //SBIW 9711 1001 0111 0001 0001
    0xC0, 0xE0, //LDI E0C0 1110 0000 1100 0000
    0xD2, 0xE0, //LDI E0D2 1110 0000 1101 0010
    0x09, 0x91, //LD 9109 (POST INCREMENTED USING Y) 1001 0001 0000 1001
    0x1E, 0x91, //LD 911E (PRE INCREMENTED USING X)  1001 0001 0001 1110
    0x01, 0x17, //CP 1701  0001 0111 0000 0001
    0x51, 0xF4, //BRNE F451 1111 0100 0101 0001
    0x0A, 0x2F, //MOV 2F0A   0010 1111 0000 1010
    0x0A, 0x95, //DEC 950A 1001 0101 0000 1010
    0x1C, 0x2F, //MOV 2F1C 0010 1111 0001 1100
    0x01, 0x17, //CP 1701    0001 0111 0000 0001
    0xB9, 0xF7, //BRNE F7B9  1111 0111 1011 1001
    0x0B, 0x2F, //MOV 2F0B   0010 1111 0000 1011
    0x1D, 0x2F, //MOV 2F1D   0010 1111 0001 1101
    0x01, 0x17, //CP 1701    0001 0111 0000 0001
    0x99, 0xF7, //BRNE F799  1111 0111 1001 1001
    0x03, 0x94, //INC 9403   1001 0100 0000 0111
    0x00, 0x00 }; //NOP 0000

const uint16_t inst16_table[] = { //tabla de instrucciones
  {0x0}, //NOP
  {0x2400}, //CLR
  {0xE000}, //LDI
  {0x910D}, //LD (POST INCREMENTED USING X)
  {0x3000}, //CPI
  {0xF7E9}, //BRNE
  {0x9711}, //SBIW
  {0x1710}, //CP
  {0x2F0A}, //MOV
  {0x950A}, //DEC
  {0x9403}, //INC
};

enum{ //constantes para acceder a la tabla
    e_NOP,
    e_CLR,
    e_LDI,
    e_LD,
    e_CPI,
    e_BRNE,
    e_SBIW,
    e_CP,
    e_MOV,
    e_DEC,
    e_INC,
};

typedef union {  //para leer dos bytes como una sola instruccion
    uint16_t op16; // e.g.: watchdog, nop
    struct{
        uint16_t op4:4;
        uint16_t d5:5;
        uint16_t op7:7;
    }type1; // e.g: LSR

    struct{
        uint16_t r4:4;
        uint16_t d5:5;
        uint16_t r1:1;
        uint16_t op6:6;
    }type2; // e.g.: MOV,MUL,ADC,ADD,AND,
    // TO-DO: Add more types as needed
    struct{
        uint16_t d10:10;
        uint16_t op6:6;
    }type3; //CLR

    struct {
        uint16_t K4_1:4;
        uint16_t d4:4;
        uint16_t K4_2:4;
        uint16_t op4:4;
    }type4; //LDI
    struct {
        uint16_t op4_1:4;
        uint16_t d5:5;
        uint16_t op7_2:7;
    }type5; //LD
    struct {
        uint16_t K4_1:4;
        uint16_t d4:4;
        uint16_t K4_2:4;
        uint16_t op4:4;
    }type6; //CPI
    struct {
        uint16_t op3_1:3;
        uint16_t K7_1:7;
        uint16_t op6:6;
    }type7; //BRNE
    struct {
        uint16_t K4_1:4;
        uint16_t d2:2;
        uint16_t K2_2:2;
        uint16_t op8:8;
    }type8; //SBIW
    struct {
        uint16_t r4_1:4;
        uint16_t d5:5;
        uint16_t r1_2:1;
        uint16_t op6:6;
    }type9; //CP
    struct {
        uint16_t r4_1:4;
        uint16_t d5:5;
        uint16_t r1_2:1;
        uint16_t op6:6;
    }type10; //MOV
    struct {
        uint16_t op4_1:4;
        uint16_t d5:5;
        uint16_t op7_2:7;
    }type11; //DEC
    struct {
        uint16_t op4_1:4;
        uint16_t d5:5;
        uint16_t op7_2:2;
    }type12; //CPI
} Op_Code_t;


int main()
{
    Op_Code_t *instruction; //puntero a una instruccion

    printf("- Practica 2: AVR OpCode -\n");

    for (uint8_t idx = 0; idx < sizeof(flash_mem); idx+=2) //recorre la memoria de 2 en 2 bytes
    {
        instruction = (Op_Code_t*) &flash_mem[idx];//direccion actual
        uint16_t op = instruction->op16; //se guarda la instruccion

        // NOP
        if (op == inst16_table[e_NOP])
        {
            printf("NOP\n");
        }

        // CLR
        else if ((op & 0xFC00) == 0x2400 && ((op >> 4) & 0x1F) == (op & 0xF))
        {
            uint8_t d = (op >> 4) & 0x1F;
            printf("CLR R%d\n", d);
        }

        // LDI
        else if ((op & 0xF000) == 0xE000)
        {
            uint8_t d = ((op >> 4) & 0xF) + 16;
            uint8_t K = ((op >> 8) & 0xF) << 4 | (op & 0xF);
            printf("LDI R%d, 0x%02X\n", d, K);
        }

        // LD
        else if ((op & 0xFE0F) == 0x900D ||
                 (op & 0xFE0F) == 0x9009 ||
                 (op & 0xFE0F) == 0x900E)
        {
            uint8_t d = (op >> 4) & 0x1F;

            if ((op & 0xF) == 0xD)
                printf("LD R%d, X+\n", d);
            else if ((op & 0xF) == 0x9)
                printf("LD R%d, Y+\n", d);
            else if ((op & 0xF) == 0xE)
                printf("LD R%d, -X\n", d);
        }

        // CPI
        else if ((op & 0xF000) == 0x3000) //extrae registro destino, valor inmediato
        {
            uint8_t d = ((op >> 4) & 0xF) + 16;
            uint8_t K = ((op >> 8) & 0xF) << 4 | (op & 0xF);
            printf("CPI R%d, 0x%02X\n", d, K);
        }

        // BRNE
        else if ((op & 0xFC07) == 0xF401)
        {
            int8_t k = (op >> 3) & 0x7F;
            if (k & 0x40)
                k |= 0x80;
            printf("BRNE %d\n", k);
        }
        //SBIW
        else if ((op & 0x9700) == 0x9700)
        {

            uint8_t d;
            if (d==1)
            d=26;
            uint8_t K = (((op & 0x00C0) >> 6) << 4) | (op & 0x000F);
            printf("SBIW R%d, 0x%02X\n", d, K);
        }
        // CP
        else if ((op & 0xFC00) == 0x1400)
        {
            uint8_t d = (op >> 4) & 0x1F;
            uint8_t r = ((op >> 9) & 0x1) << 4 | (op & 0xF);
            printf("CP R%d, R%d\n", d, r);
        }

        // MOV
        else if ((op & 0xFC00) == 0x2C00)
        {
            uint8_t d = (op >> 4) & 0x1F;
            uint8_t r = ((op >> 9) & 0x1) << 4 | (op & 0xF);
            printf("MOV R%d, R%d\n", d, r);
        }

        // DEC
        else if ((op & 0xFE0F) == 0x940A)
        {
            uint8_t d = (op >> 4) & 0x1F;
            printf("DEC R%d\n", d);
        }

        // INC
        else if ((op & 0xFE0F) == 0x9403)
        {
            uint8_t d = (op >> 4) & 0x1F;
            printf("INC R%d\n", d);
        }

        else
        {
            printf("unknown (0x%04X)\n", op);
        }
    }

    return 0;
}



/******************************************************************************
Prac 2 - AVR ASM OpCode Decoder
*******************************************************************************/

#include <stdio.h>
#include <inttypes.h>

const uint8_t flash_mem[] ={ 
    0x00, 0x24, 0xA0, 0xE0, 0xB2, 0xE0, 0x0D, 0x91, 0x00, 0x30, 0xE9, 0xF7, 0x11, 0x97, 0xC0, 0xE0, 0xC4, 
    0xD2, 0xE0, 0x09, 0x91, 0x1E, 0x91, 0x01, 0x17, 0x51, 0xF4, 0x0A, 0x2F, 0x0A, 0x95, 0x1C, 0x2F, 0x65, 
    0x01, 0x17, 0xB9, 0xF7, 0x0B, 0x2F, 0x1D, 0x2F, 0x01, 0x17, 0x99, 0xF7, 0x03, 0x94, 0x00, 0x00 };

const uint16_t inst16_table[] = {
  {0x0}, //NOP
};

enum{
    e_NOP,
};


// Op Code struct
typedef union {
    uint16_t op16; // e.g.: watchdog, nop
    struct{
        uint16_t op4:4;
        uint16_t d5:5;
        uint16_t op7:7;
    }type1; // e.g: LSR
    struct{
        uint16_t r4:4;
        uint16_t d5:5;
        uint16_t r1:1;
        uint16_t op6:6;
    }type2; // e.g.: MOV,MUL,ADC,ADD,AND,
    // TO-DO: Add more types as needed
} Op_Code_t;


int main()
{
    Op_Code_t *instruction;
    printf("- Practica 2: AVR OpCode -\n");
    // Decode the instructions by cycling through the array
    for (uint8_t idx = 0; idx < sizeof(flash_mem); idx+=2)
    {
        instruction = (Op_Code_t*) &flash_mem[idx];
        if (instruction->op16 == inst16_table[e_NOP])
        {
            printf("NOP\n");
        }
        else
        {
            printf("unknown\n");
        }
    }
    return 0;
}
