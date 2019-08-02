#include <stdint.h>

#include "6502.h"
#include "memory.h"
#include "mapper.h"

#define VERIFY_SP(x) \
    x->sp &= 0xFF;   \
    x->sp += 0x100

uint8_t get_stack_byte(core6502_t *core)
{
    core->pc--;
    VERIFY_SP(core);
    return get_byte(core, core->pc);
}

uint16_t get_stack_short(core6502_t *core)
{
    core->sp -= 2;
    VERIFY_SP(core);
    return get_short(core, core->sp);
}

void put_stack_byte(core6502_t *core, uint8_t byte)
{
    put_byte(core, core->sp++, byte);
    VERIFY_SP(core);
}

void put_stack_short(core6502_t *core, uint16_t value)
{
    put_byte(core, core->sp++, value & 0xFF);
    VERIFY_SP(core);
    put_byte(core, core->sp++, value >> 8);
    VERIFY_SP(core);
}

uint8_t get_byte(core6502_t *core, uint16_t addr)
{
    return mapped_read(core->mapper, addr);
}

uint16_t get_short(core6502_t *core, uint16_t addr)
{
    uint16_t first = get_byte(core, addr);
    return first + (get_byte(core, addr + 1) << 8);
}

void put_byte(core6502_t *core, uint16_t addr, uint8_t byte)
{
    mapped_write(core->mapper, addr, byte);
}

uint8_t get_pc_byte(core6502_t *core)
{
    return mapped_read(core->mapper, core->pc++);
}

uint16_t get_pc_short(core6502_t *core)
{
    uint16_t first = get_pc_byte(core);
    return first + (get_pc_byte(core) << 8);
}