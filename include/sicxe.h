#ifndef SICXE_H
#define SICXE_H

/**
 * Minimal SIC/XE architecture definitions for the relocating loader.
 *
 * This header provides:
 *   - Basic enums and constants for SIC/XE instruction formats (1, 2, 3, 4)
 *   - A place to add SIC/XE-specific flags or addressing-mode helpers
 *
 * Used primarily by reloc_sicxe.c to interpret and update relocated addresses
 * in SIC/XE object code.
 */


typedef enum {
    FORMAT1 = 1,
    FORMAT2 = 2,
    FORMAT3 = 3,
    FORMAT4 = 4
} SicXeFormat;

#endif
