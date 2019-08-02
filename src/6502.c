#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "6502.h"
#include "memory.h"

void do_opcode_6502(core6502_t *core) {
    uint8_t opcode = get_pc_byte(core);
    
    if (opcode == 0x00) {
        // Control Instructions
        if (opcode == 0b00000000) {
            // BRK Instruction
            core->flag |= BRK;
            put_stack_short(core, core->pc);
            put_stack_byte(core, core->flag);
            core->pc = get_short(core, 0xFFFE);
        } else if (opcode == 0x08) {
            // PHP - Push Status
            put_stack_byte(core, core->flag);
        } else if (opcode == 0x10) {
            // BPL - Branch Positive
            uint8_t offset = get_pc_byte(core);
            
            if ((core->flag & NEG) == 0x00)
                core->pc += offset;
        } else if (opcode == 0x18) {
            // CLC - Clear Carry
            core->flag &= ~CAR;
        } else if (opcode == 0x20) {
            // JSR - Jump to subroutine
            uint16_t dest = get_pc_short(core);
            put_stack_short(core, core->pc);
            core->pc = dest - 1;
        } else if (opcode == 0x24 || opcode == 0x2C) {
            // BIT - Bit test
            uint16_t addr;
            if (opcode == 0x24)
                addr = get_pc_byte(core);
            else 
                addr = get_byte(core, get_pc_short(core));
            uint8_t byte = get_byte(core, addr);
            core->flag &= ~(ZER | OVF | NEG);
            if ((byte & core->a) == 0x00)
                core->flag |= ZER;
            core->flag |= (byte & (OVF | NEG));
        } else if (opcode == 0x28) {
            // PLP - Pull proccessor status
            core->flag = get_stack_byte(core);
        } else if (opcode == 0x30) {
            // BMI - Branch on Minus
            uint8_t offset = get_pc_byte(core);

            if (core->flag & NEG)
                core->pc += offset;
        } else if (opcode == 0x38) {
            // SEC - Set Carry
            core->flag |= CAR;
        } else if (opcode == 0x40) {
            // RTI - Return from Interupt
            core->flag = get_stack_byte(core);
            core->pc = get_stack_short(core);
        } else if (opcode == 0x48) {
            // PHA - Push A
            put_stack_byte(core, core->a);
        } else if (opcode == 0x4C || opcode == 0x6C) {
            // JMP - Jump
            uint16_t addr = get_pc_short(core);
            if (opcode == 0x6C)
                addr = get_short(core, addr);
            core->pc = addr;
        } else if (opcode == 0x50) {
            // BVC - Branch Overflow Clear
            uint8_t offset = get_pc_byte(core);

            if ((core->flag & NEG) == 0x00)
                core->pc += offset;
        } else if (opcode == 0x58) {
            // CLI - Clear Interupt Flag
            core->flag &= ~INT; 
        } else if (opcode == 0x60) {
            // RTS -  Return Subroutine
            uint16_t addr = get_stack_short(core) + 1;
            core->pc = addr;
        } else if (opcode == 0x68) {
            // PLA - Pull A from stack
            core->a = get_stack_byte(core);
        } else if (opcode == 0x70) {
            // BVS - Branch overflow set
            uint8_t offset = get_pc_byte(core);

            if (core->flag & OVF)
                core->pc += offset;
        } else if (opcode == 0x78) {
            // SEI - Set Interupt Enable
            core->flag |= INT;
        } else if (opcode == 0x84 || opcode == 0x94 || opcode == 0x8C) {
            // STY - Store Y
            uint16_t addr;
            if (opcode == 0x84)
                addr = get_pc_byte(core);
            else if (opcode == 0x94)
                addr = get_pc_byte(core) + core->y;
            else
                addr = get_pc_short(core);
            put_byte(core, addr, core->y);
        } else if (opcode == 0x88) {
            // DEY - Decrement Y
            core->y--;
        } else if (opcode == 0x90) {
            // BCC - Branch Carry Clear
            uint8_t offset = get_pc_byte(core);
            if ((core->flag & CAR) == 0x00)
                core->pc += offset;
        } else if (opcode == 0x98) {
            // TYA - Tansfer Y to A
            core->a = core->y;
        } else if (opcode == 0xA0 || opcode == 0xA4 || opcode == 0xB4 || opcode == 0xAC || opcode == 0xBC) {
            // LDY - Load Y Register
            uint16_t addr;
            if (opcode == 0xA0)
                addr = core->pc++;
            else if (opcode == 0xA4)
                addr = get_pc_byte(core);
            else if (opcode == 0xB4)
                addr = get_pc_byte(core) + core->x;
            else if (opcode == 0xAC)
                addr = get_pc_short(core);
            else
                addr = get_pc_short(core) + core->x;
            core->y = get_byte(core, addr);
        } else if (opcode == 0xA8) {
            // TAY - Transfer A to Y
            core->y = core->a;
        } else if (opcode == 0xB0) {
            // BCS - Branch carry Set
            uint8_t offset = get_pc_byte(core);
            if (core->flag & CAR)
                core->pc += offset;
        } else if (opcode == 0xB8) {
            // CLV - Clear Overflow
            core->flag &= ~OVF;
        } else if (opcode == 0xC0 || opcode == 0xC4 || opcode == 0xCC) {
            // CPY - Compare Y
            uint16_t addr;
            if (opcode == 0xC0)
                addr = core->pc++;
            else if (opcode == 0xC4)
                addr = get_pc_byte(core);
            else
                addr = get_pc_short(core);

            uint8_t operand = get_byte(core, addr);
            
            // Clear Flags
            core->flag &= ~(CAR | ZER | NEG);

            // Check Carry Flag
            if (core->y >= operand)
                core->flag |= CAR;

            // Set Flags
            if (core->y == operand) core->flag |= ZER;
            if ((core->y - operand) & 0b10000000) core->flag |= NEG;
        } else if (opcode == 0xC8) {
            // INY - Increment Y
            core->y++;
        } else if (opcode == 0xD0) {
            // BNE - Branch Not-Equal
            uint8_t offset = get_pc_byte(core);
            if ((core->flag & ZER) == 0x00)
                core->pc += offset;
        } else if (opcode == 0xE0 || opcode == 0xE4 || opcode == 0xEC) {
            // CPX - Compare X
            uint16_t addr;
            if (opcode == 0xE0)
                addr = core->pc++;
            else if (opcode == 0xE4)
                addr = get_pc_byte(core);
            else
                addr = get_pc_short(core);

            uint8_t operand = get_byte(core, addr);

            // Clear Flags
            core->flag &= ~(CAR | ZER | NEG);

            // Check Carry Flag
            if (core->x >= operand)
                core->flag |= CAR;

            // Set Flags
            if (core->x == operand) core->flag |= ZER;
            if ((core->x - operand) & 0b10000000) core->flag |= NEG;
        } else if (opcode == 0xE8) {
            // INX - Increment X
            core->x--;
        } else if (opcode == 0xF0) {
            // BEQ - Branch Equal
            uint8_t offset = get_pc_byte(core);
            if (core->flag & ZER)
                core->pc += offset;
        }
    } else if ((opcode & 0b00000011) == 0b00000001) {
        // ALU Instructions
        
        uint16_t addr;
        
        // Get operand address
        if ((opcode & 0b00011100) == 0b00000000) {
            // Indirect, X
            addr = get_short(core, (get_pc_byte(core) + core->x) & 0xff);
        } else if ((opcode & 0b00011100) == 0b00000100) {
            // Zero Page
            addr = get_pc_byte(core);
        } else if ((opcode & 0b00011100) == 0b00001000) {
            // Immediate
            addr = core->pc++;
        } else if ((opcode & 0b00011100) == 0b00001100) {
            // Absolute
            addr = get_pc_short(core);
        } else if ((opcode & 0b00011100) == 0b00010000) {
            // Indirect, Y
            addr = get_short(core, get_pc_byte(core)) + core->y;
        } else if ((opcode & 0b00011100) == 0b00010100) {
            // Zero Page, X
            addr = get_pc_byte(core) + core->x;
            printf("%x\n", addr);
        } else if ((opcode & 0b00011100) == 0b00011000) {
            // Absolute, Y
            addr = get_pc_short(core) + core->y;
        } else if ((opcode & 0b00011100) == 0b00011100) {
            // Absolute, X
            addr = get_pc_short(core) + core->x;
        }
        
        // Instruction Decode
        if ((opcode & 0b11100000) == 0b00000000) {
            // ORA, Bitwise OR with A (S, Z)
            
            core->a |= get_byte(core, addr);
            
            // Clear Flags
            core->flag &= ~(ZER | NEG);
            
            // Set Flags
            if (core->a == 0) core->flag |= ZER;
            if (core->a & 0b10000000) core->flag |= NEG;
            
        } else if ((opcode & 0b11100000) == 0b00100000) {
            // AND, Bitwise AND with A (S, Z)
            
            core->a &= get_byte(core, addr);
            
            // Clear Flags
            core->flag &= ~(ZER | NEG);
            
            // Set Flags
            if (core->a == 0) core->flag |= ZER;
            if (core->a & 0b10000000) core->flag |= NEG;
            
        } else if ((opcode & 0b11100000) == 0b01000000) {
            // EOR, Bitwise XOR with A (S, Z)
            
            core->a ^= get_byte(core, addr);
            
            // Clear Flags
            core->flag &= ~(ZER | NEG);
            
            // Set Flags
            if (core->a == 0) core->flag |= ZER;
            if (core->a & 0b10000000) core->flag |= NEG;
            
        } else if ((opcode & 0b11100000) == 0b01100000) {
            // ADC, Add with Carry (S, V, Z, C)
            
            uint16_t operand = get_byte(core, addr);
            
            if (core->flag & CAR) operand += 1;
            
            // Clear Flags
            core->flag &= ~(OVF | CAR | ZER | NEG);
            
            // Check for overflow flag
            if (core->a >= 0x80 && operand >= 0x80 && core->a + operand >= 0x100)
                core->flag |= OVF;
            else if (core->a < 0x80 && operand < 0x80 && core->a + operand >= 0x80)
                core->flag |= OVF;
            
            // Check Carry Flag
            if (core->a + operand >= 0x0100)
                core->flag |= CAR;
            
            core->a += operand;
            
            // Set Flags
            if (core->a == 0) core->flag |= ZER;
            if (core->a & 0b10000000) core->flag |= NEG;
            
        } else if ((opcode & 0b11100000) == 0b10000000) {
            // STA, Store A in memory (NONE)
            
            put_byte(core, addr, core->a);
            
        } else if ((opcode & 0b11100000) == 0b10100000) {
            // LDA, Load A from memory (S, Z)
            
            core->a = get_byte(core, addr);
            
            // Clear Flags
            core->flag &= ~(ZER | NEG);
            
            // Set Flags
            if (core->a == 0) core->flag |= ZER;
            if (core->a & 0b10000000) core->flag |= NEG;
            
        } else if ((opcode & 0b11100000) == 0b11000000) {
            // CMP, Subtract with carry (S, V, Z, C) A-M
            
            uint8_t operand = get_byte(core, addr);
            
            // Clear Flags
            core->flag &= ~(CAR | ZER | NEG);
            
            // Check Carry Flag
            if (core->a >= operand)
                core->flag |= CAR;
            
            // Set Flags
            if (core->a == operand) core->flag |= ZER;
            if ((core->a - operand) & 0b10000000) core->flag |= NEG;
            
        } else if ((opcode & 0b11100000) == 0b11100000) {
            // SBC, Subtract with carry (S, V, Z, C) A-M-(1-C)
            
            uint16_t operand = (~get_byte(core, addr)) + 1;
            
            if (!(core->flag & CAR)) operand += 1;
            
            // Clear Flags
            core->flag &= ~(OVF | CAR | ZER | NEG);
            
            // Check for overflow flag
            if (core->a >= 0x80 && operand >= 0x80 && core->a + operand >= 0x100)
                core->flag |= OVF;
            else if (core->a < 0x80 && operand < 0x80 && core->a + operand >= 0x80)
                core->flag |= OVF;
            
            // Check Carry Flag
            if (core->a + operand >= 0x0100)
                core->flag |= CAR;
            
            core->a += operand;
            
            // Set Flags
            if (core->a == 0) core->flag |= ZER;
            if (core->a & 0b10000000) core->flag |= NEG;
        }
    } else if ((opcode & 0b00000011) == 0b00000010) {
        uint16_t addr;
        
        // RMW Instructions
        if ((opcode & 0b11100000) == 0b00000000) {
            // ASL - Aritmetic Shift Left
            uint8_t operand;
            if ((opcode & 0x1F) == 0x0A)
                operand = core->a;
            else 
                operand = get_byte(core, addr);
            core->flag &= ~CAR;
            core->flag |= operand & 0b10000000;
            operand <<= 1;
            if ((opcode & 0x1F) == 0x0A)
                core->a = operand;
            else
                put_byte(core, addr, operand);
        } else if ((opcode & 0b11100000) == 0b00100000) {
            // ROL - Rotate Left
            uint8_t operand;
            if ((opcode & 0x1F) == 0x0A)
                operand = core->a;
            else
                operand = get_byte(core, addr);

            uint8_t carry = core->flag & CAR ? 1 : 0;
            core->flag &= ~(ZER | CAR | NEG);
            core->flag |= operand & 0b10000000;
            operand <<= 1;
            operand += carry;
            
            if ((opcode & 0x1F) == 0x0A)
                core->a = operand;
            else
                put_byte(core, addr, operand);
        } else if ((opcode & 0b11100000) == 0b01000000) {
            // LSR - Logical Shift Right
            uint8_t operand;
            if ((opcode & 0x1F) == 0x0A)
                operand = core->a;
            else
                operand = get_byte(core, addr);

            core->flag &= ~(ZER | CAR | NEG);
            core->flag |= operand & 0b00000001;
            operand >>= 1;
            
            if ((opcode & 0x1F) == 0x0A)
                core->a = operand;
            else
                put_byte(core, addr, operand);
        } else if ((opcode & 0b11100000) == 0b01100000) {
            // ROR - Rotate Right
            uint8_t operand;
            if ((opcode & 0x1F) == 0x0A)
                operand = core->a;
            else
                operand = get_byte(core, addr);

            int8_t carry = core->flag & CAR ? 0b10000000 : 0b00000000;
            core->flag &= ~(ZER | CAR | NEG);
            core->flag |= operand & 0b10000000;
            operand >>= 1;
            operand += carry;

            if ((opcode & 0x1F) == 0x0A)
                core->a = operand;
            else
                put_byte(core, addr, operand);
        } else if ((opcode & 0b11100000) == 0b10000000) {
            // STX - Store X
            uint16_t addr;

            if (opcode == 0x8A)
                core->a = core->x;
            else if (opcode == 0x9A)
                core->sp = core->x;
            else
                put_byte(core, addr, core->x);
        } else if ((opcode & 0b11100000) == 0b10100000) {
            // LDX - Load X
            uint16_t addr;

            if (opcode == 0xAA)
                core->x = core->a;
            else if (opcode == 0xBA)
                core->x = core->sp;
            else
                core->x = get_byte(core, addr);

        } else if ((opcode & 0b11100000) == 0b11000000) {
            // DEC - Decrement
            uint8_t operand;
            operand--;

            // Clear Flags
            core->flag &= ~(ZER | NEG);
            
            // Set Flags
            if (core->a == 0) core->flag |= ZER;
            if (core->a & 0b10000000) core->flag |= NEG;
        } else if ((opcode & 0b11100000) == 0b11100000) {
            // INC - Increment
            uint8_t operand;
            operand++;

            // Clear Flags
            core->flag &= ~(ZER | NEG);

            // Set Flags
            if (core->a == 0) core->flag |= ZER; 
            if (core->a & 0b10000000) core->flag |= NEG;
        }
    } else if ((opcode & 0b00000011) == 0b00000011) {
        // Combo Instructions (Here be dragons)
        // Skipping for Now
    }
}