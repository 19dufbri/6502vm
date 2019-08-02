#include <stdint.h>
#include <mapper.h>

#ifndef M6502_H
#define M6502_H

enum core6502flags
{
    CAR = 0b00000001,
    ZER = 0b00000010,
    INT = 0b00000100,
    BCD = 0b00001000,
    BRK = 0b00010000,
    FRE = 0b00100000,
    OVF = 0b01000000,
    NEG = 0b10000000
};

typedef struct core6502
{
    uint8_t a;
    uint8_t x;
    uint8_t y;
    uint16_t sp;
    uint16_t pc;
    uint8_t flag;
    mapper_t *mapper;
} core6502_t;

void do_opcode_6502(core6502_t *core);

#endif