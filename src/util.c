#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/** 
 * Implementation of shared utility functions for the relocating loader.
 *
 * This file implements:
 *   - Implement parseHex(), which converts a hex string into a
 *     uint32_t (optionally handling an optional 0x/0X prefix)
 *   - Implement fatal(), which prints an error message and
 *     terminates the program with a non-zero exit code
 *
 * These helpers centralize common tasks so that main.c, loader.c,
 * parser, and relocation code can remain clean and focused on their
 * core logic.
 */


int parseHex (const char *s, uint32_t *out) {
    if (s == NULL || *s == '\0' || out == NULL) {
        return 0;
    }

    // Skip optional 0x / 0X
    if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) {
        s += 2;
    }

    uint32_t value = 0;
    while (*s != '\0') {
        char c = (char)*s;
        int digit;

        if (c >= '0' && c <= '9') {
            digit = c - '0';
        } else if (c >= 'a' && c <= 'f') {
            digit = 10 + (c - 'a');
        } else if (c >= 'A' && c <= 'F') {
            digit = 10 + (c - 'A');
        } else if (isspace((unsigned char)c)) {
            // Allow trailing whitespace
            s++;
            continue;
        } else {
            return 0; // invalid char
        }

        // Shift left 4 bits and add digit
        value = (value << 4) | (uint32_t)digit;
        s++;
    }

    *out = value;
    return 1;
}

void fatal(const char *msg) {
    if (msg == NULL) {
        msg = "fatal error";
    }
    printf("Error: %s\n", msg);
    exit(EXIT_FAILURE);
}
