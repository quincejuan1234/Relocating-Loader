#ifndef SIC_H
#define SIC_H

/**
 * Architecture constants for the classic SIC machine.
 *
 * This header provides:
 *   - Word size, address width, and maximum memory size for SIC
 *   - Any SIC-specific constants needed by the relocation code
 *
 * Used primarily by relocSic.c and memory.c.
 */


#define SIC_WORD_SIZE 3       // bytes
#define SIC_ADDR_BITS 24      // 24-bit addressing
#define SIC_MAX_MEMORY 32768  // 32 KB

#endif
