#include "6502.h"
#include <stdio.h>
#include <stdint.h>
#include <assert.h>

void debug_print_status(core6502_t *core) {
    printf("Status:\n");
    printf("a:\t0x%02x\n", core->a);
    printf("x:\t0x%02x\n", core->x);
    printf("y:\t0x%02x\n", core->y);
    printf("sp:\t0x%04x\n", core->sp);
    printf("pc:\t0x%04x\n", core->pc);
    printf("Flags:\t");
    printf(core->flag & NEG ? "N" : "-");
    printf(core->flag & OVF ? "V" : "-");
    printf(core->flag & FRE ? "F" : "-");
    printf(core->flag & BRK ? "B" : "-");
    printf(core->flag & BCD ? "D" : "-");
    printf(core->flag & INT ? "I" : "-");
    printf(core->flag & ZER ? "Z" : "-");
    printf(core->flag & CAR ? "C" : "-");
    printf("\n");
}

uint8_t *mem;

uint8_t test_read(uint16_t addr) {
    // printf("Memory Read at: \t0x%04x\n", addr);
    return mem[addr];
}

void test_write(uint16_t addr, uint8_t val) {
    // putchar((char) val);
    mem[addr] = val;
}

uint8_t test_read_in(uint16_t addr) {
    uint8_t val = '\n';
    while (val == '\n')
        val = getchar();
    return val;
}

void test_write_out(uint16_t addr, uint8_t val) {
    putchar((char) val);
}
/*
=== Adressing Modes:
Indirect, X
Zero Page
Immediate
Absolute
Indirect, Y
Zero Page, X
Absolute, Y
Absolute, X

=== Instructions:
ORA
AND
EOR
ADC
STA
LDA
CMP
    SBC
*/

#define CHECKFLAG(x, y) (assert(x->flag & y))
#define CHECKVALUE(x,y) (assert(x == y))

void adc_check(core6502_t *core) {
    printf("===== Beginning ADC Checks =====\n");
    printf("Checks ALU functionality\n");

    mem[0x0000] = 0x69; // Add Immediate
    mem[0x0001] = 0x80; // 0x80 + 0x80 = 0
    mem[0x0002] = 0x69; // Add Immediate
    mem[0x0003] = 0x00; // 0x00 + 0x00 = 0
    mem[0x0004] = 0x69; // Add Immediate
    mem[0x0005] = 0x01; // 0x00 + 0x01 = 1
    mem[0x0006] = 0x69; // Add Immediate
    mem[0x0007] = 0x00; // 0x01 + 0x00 + C = 2
    mem[0x0008] = 0x69; // Add Immediate
    mem[0x0009] = 0x00; // 0xFF + 0x00 + C = 0x0
    mem[0x000A] = 0x69; // Add Immediate
    mem[0x000B] = 0x00; // 0xFE + 0x00 + C = 0xFF

    core->a =       0x80;
    core->x =       0x00;
    core->y =       0x00;
    core->pc =      0x0000;
    core->sp =      0x0000;
    
    core->flag =    FRE;
    do_opcode_6502(core);
    CHECKVALUE(core->a, 0x00);
    CHECKFLAG(core, ~NEG);
    CHECKFLAG(core, OVF);
    CHECKFLAG(core, CAR);
    CHECKFLAG(core, ZER);
    
    core->flag =    FRE;
    do_opcode_6502(core);
    CHECKVALUE(core->a, 0x00);
    CHECKFLAG(core, ~NEG);
    CHECKFLAG(core, ~OVF);
    CHECKFLAG(core, ~CAR);
    CHECKFLAG(core, ZER);
    
    core->flag =    FRE;
    do_opcode_6502(core);
    CHECKVALUE(core->a, 0x01);
    CHECKFLAG(core, ~NEG);
    CHECKFLAG(core, ~OVF);
    CHECKFLAG(core, ~CAR);
    CHECKFLAG(core, ~ZER);
    
    core->flag =    FRE | CAR;
    core->a =       0x01;
    do_opcode_6502(core);
    CHECKVALUE(core->a, 0x02);
    CHECKFLAG(core, ~NEG);
    CHECKFLAG(core, ~OVF);
    CHECKFLAG(core, ~CAR);
    CHECKFLAG(core, ~ZER);
    
    core->flag =    FRE | CAR;
    core->a =       0xFF;
    do_opcode_6502(core);
    CHECKVALUE(core->a, 0x00);
    CHECKFLAG(core, ~NEG);
    CHECKFLAG(core, ~OVF);
    CHECKFLAG(core, CAR);
    CHECKFLAG(core, ZER);
    
    core->flag =    FRE | CAR;
    core->a =       0xFE;
    do_opcode_6502(core);
    CHECKVALUE(core->a, 0xFF);
    CHECKFLAG(core, NEG);
    CHECKFLAG(core, ~OVF);
    CHECKFLAG(core, ~CAR);
    CHECKFLAG(core, ~ZER);
    printf("===== Completed ADC Checks =====\n");
}

void print_check(core6502_t *core) {
    printf("==== Starting Printer Checks ====\n");
    printf("Checks mapper.c, linked_list.c, and ALU\n");    

    mem[0x0000] = 0xA9; // LDA #i
    mem[0x0001] = 0x41; // 'A'
    mem[0x0002] = 0x8D; // STA a
    mem[0x0003] = 0x00; // Address Low Byte
    mem[0x0004] = 0x10; // Address High Byte
    
    core->a =       0x00;
    core->x =       0x00;
    core->y =       0x00;
    core->pc =      0x0000;
    core->sp =      0x0000;
    
    debug_print_status(core);
    do_opcode_6502(core); // LDA 'A'        Not offical 6502 ASM
    debug_print_status(core);
    do_opcode_6502(core); // STA 0x1000     syntax but whatevs...
    debug_print_status(core);

    for (int i = 0; i < 10; i++) {
        mem[core->pc] =   0xAD; // LDA 0x1000
        mem[core->pc+1] = 0x00;
        mem[core->pc+2] = 0x10;
        mem[core->pc+3] = 0x8D; // STA 0x1000
        mem[core->pc+4] = 0x00;
        mem[core->pc+5] = 0x10;
        do_opcode_6502(core);
        do_opcode_6502(core);
        printf("\n");
    }

    printf("==== Finished Printer Checks ====\n");
}

int main() {
    core6502_t *core = malloc(sizeof(core6502_t));
    mem = calloc(0x1000, sizeof(uint8_t));
    
    core->a =       0x00;
    core->x =       0x00;
    core->y =       0x00;
    core->pc =      0x0000;
    core->sp =      0x0000;
    core->flag =    FRE;
    
    mapping_t *mapping = malloc(sizeof(mapping_t));
    mapping->start = 0x0;
    mapping->len = 0x1000;
    mapping->read_func = test_read;
    mapping->write_func = test_write;
   
    mapping_t *mapping_p = malloc(sizeof(mapping_t));
    mapping_p->start = 0x1000;
    mapping_p->len = 0x100;
    mapping_p->read_func = test_read_in;
    mapping_p->write_func = test_write_out;

    core->mapper = new_mapper();
    int mapping_handle = add_mapping(core->mapper, mapping);
    int mapping_p_handle = add_mapping(core->mapper, mapping_p);

    printf("==== Finished Initialization ====\n");
    debug_print_status(core);
    
    printf("==== Starting Tests ====\n");
    adc_check(core);
    print_check(core);
    printf("==== Finished Tests ====\n");

    printf("==== Started Cleanup ====\n");
    del_mapping(core->mapper, mapping_handle);
    free(mapping);
    free(mem);
    del_mapper(core->mapper);
    free(core);
    printf("==== Finished Cleanup ====\n");
}
