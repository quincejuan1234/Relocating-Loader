#include "relocSic.h"
#include "memory.h"
#include "objFile.h"
#include "util.h"

#include <stddef.h>

// Made byy Miguel Aponte (n01557736)

/**
 * Relocation logic for classic SIC object programs.
 *
 * Responsibilities:
 *   - Implement relocateSic(), which:
 *       * Receives a parsed objFile and a relocation address
 *       * Computes the relocation factor (newStart - originalStart)
 *       * Walks all SIC modification records (M)
 *       * Reads the affected word(s) and adds the relocation factor
 *       * Ensures the relocated values fit within SIC 24-bit addresses
 *   - Update the header/end records or any other fields as required
 *     by the project spec for relocated programs
 *
 * This module assumes the object file contents are already parsed by
 * scoff_parser.c and focuses only on adjusting addresses for SIC.
 */


void relocateSic(objFile *obj, uint32_t reloc) {
    if (!obj) {
        fatal("relocateSic: NULL objFile pointer");
    }

    uint32_t oldStart = obj->header.startAddress;
    int32_t  R        = (int32_t)reloc - (int32_t)oldStart;

    memInit();

    for (size_t i = 0; i < obj->textCount; i++) {
        textRecord *t = &obj->textRecords[i];

        for (uint32_t j = 0; j < t->length; j++) {
            uint32_t addr = t->address + j;
            memWriteByte(addr, t->bytes[j]);
        }
    }

    for (size_t i = 0; i < obj->modCount; i++) {
        modRecord *m = &obj->modRecords[i];

        uint32_t val = memReadWord(m->address);

        if (m->sign == '+') {
            val += (uint32_t)R;
        } else if (m->sign == '-') {
            val -= (uint32_t)R;
        } else {
            fatal("relocateSic: invalid sign in modification record (expected '+' or '-')");
        }

        val &= 0xFFFFFFu;

        memWriteWord(m->address, val);
    }
} 
