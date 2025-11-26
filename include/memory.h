#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>

#define MEM_SIZE 32768  // 32K bytes for SIC

void memInit(void);
void memWriteByte(uint32_t addr, uint8_t value);
uint8_t memReadByte(uint32_t addr);
void memWriteWord(uint32_t addr, uint32_t value); // 3-byte SIC word
uint32_t memReadWord(uint32_t addr);

#endif
