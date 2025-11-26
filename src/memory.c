#include "memory.h"
#include "util.h"

/** 
 * Simple simulated memory implementation for the relocating loader.
 *
 * Responsibilities:
 *   - Provide a fixed-size byte array representing SIC memory
 *   - Implement memInit() to clear memory
 *   - Implement memWriteByte() / memReadByte() with bounds checking
 *   - Implement memWriteWord() / memReadWord() for 3-byte SIC words
 *
 * Optional design:
 *   - The loader and relocation modules may choose to load all text
 *     records into this memory image, apply modifications to memory,
 *     and then regenerate T records from memory for output.
 *   - Alternatively, the project can skip this abstraction and
 *     operate directly on textRecord buffers.
 */

static uint8_t memory[MEM_SIZE];

void memInit(void) {
    for (uint32_t i = 0; i < MEM_SIZE; i++) {
        memory[i] = 0;
    }
}

void memWriteByte(uint32_t addr, uint8_t value) {
    if (addr >= MEM_SIZE) {
        fatal("Memory write out of range");
    }
    memory[addr] = value;
}

uint8_t memReadByte(uint32_t addr) {
    if (addr >= MEM_SIZE) {
        fatal("Memory read out of range");
    }
    return memory[addr];
}

void memWriteWord(uint32_t addr, uint32_t value) {
    // SIC word = 3 bytes
    memWriteByte(addr,     (value >> 16) & 0xFF);
    memWriteByte(addr + 1, (value >> 8)  & 0xFF);
    memWriteByte(addr + 2,  value        & 0xFF);
}

uint32_t memReadWord(uint32_t addr) {
    uint32_t b1 = memReadByte(addr);
    uint32_t b2 = memReadByte(addr + 1);
    uint32_t b3 = memReadByte(addr + 2);
    return (b1 << 16) | (b2 << 8) | b3;
}
