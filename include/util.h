#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>

/**
 * Utility declarations shared across the relocating loader project.
 *
 * This header declares helpers for:
 *   - Parsing unsigned 32-bit hexadecimal integers from strings
 *   - Reporting fatal errors and terminating the program
 *
 * Implemented in util.c and used by main.c, loader.c, parser,
 * and relocation modules.
 */

int parseHex (const char *s, uint32_t *out);
void fatal(const char *msg);

#endif
