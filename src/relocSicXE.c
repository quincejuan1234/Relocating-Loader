#include "relocSicXE.h"
#include "memory.h"
#include "objFile.h"
#include "util.h"

#include <stddef.h>

// Made by MIguel Aponte (n01557736)

/** 
 * Relocation logic for SIC/XE object programs.
 *
 * Responsibilities:
 *   - Implement relocateSicXE(), which:
 *       * Receives a parsed objFile and a relocation address
 *       * Computes and applies the relocation factor using SIC/XE rules
 *       * Processes modification records (M) that indicate which
 *         fields in the text should be adjusted
 *       * Handles SIC/XE-specific field sizes and formats (e.g., 20-bit
 *         address fields in format 3/4 instructions, extended format)
 *   - Ensure that updated fields remain consistent with SIC/XE encoding
 *
 * Like relocSic.c, this module assumes parsing is already done and
 * focuses solely on address adjustment for SIC/XE.
 */


void relocateSicXE(objFile *obj, uint32_t reloc) {
    if (!obj) {
        fatal("relocateSicXE: NULL objFile pointer");
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

        uint32_t word = memReadWord(m->address);

        uint8_t halfBytes = m->lengthNibbles;
        uint8_t bits      = (uint8_t)(halfBytes * 4);

        if (bits == 0 || bits > 24) {
            fatal("relocateSicXE: invalid modification length in nibbles");
        }

        uint32_t mask = (1u << bits) - 1u;

        uint32_t field = word & mask;

        if (m->sign == '+') {
            field += (uint32_t)R;
        }
        else if (m->sign == '-') {
            field -= (uint32_t)R;
        }
        else {
            fatal("relocateSicXE: invalid sign in modification record (expected '+' or '-')");
        }

        field &= mask;

        word = (word & ~mask) | field;

        word &= 0xFFFFFFu;

        memWriteWord(m->address, word);
    }
}
