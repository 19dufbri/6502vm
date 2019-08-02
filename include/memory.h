#include <stdint.h>

#include "6502.h"

#ifndef MEMORY_H
#define MEMORY_H

uint8_t get_stack_byte(core6502_t *core);
uint16_t get_stack_short(core6502_t *core);

void put_stack_byte(core6502_t *core, uint8_t byte);
void put_stack_short(core6502_t *core, uint16_t value);

uint8_t get_byte(core6502_t *core, uint16_t addr);
uint16_t get_short(core6502_t *core, uint16_t addr);

void put_byte(core6502_t *core, uint16_t addr, uint8_t byte);

uint8_t get_pc_byte(core6502_t *core);
uint16_t get_pc_short(core6502_t *core);

#endif