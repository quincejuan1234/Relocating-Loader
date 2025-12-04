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

        uint32_t loadAddress = t->address + (uint32_t)R;

        for (uint32_t j = 0; j < t->length; j++) {
            uint32_t addr = loadAddress + j;
            memWriteByte(addr, t->bytes[j]);
        }
    }

    for (size_t i = 0; i < obj->modCount; i++) {
        modRecord *m = &obj->modRecords[i];

        if (m->lengthNibbles == 0) {
            fatal("relocateSicXE: invalid modification length (must be > 0 nibbles)");
        }

        uint8_t  byteCount = (uint8_t)((m->lengthNibbles + 1) / 2); // round up
        uint8_t  unusedLow = (uint8_t)(byteCount * 2U - m->lengthNibbles); // 0 or 1
        uint8_t  shift     = (uint8_t)(unusedLow * 4U);
        uint8_t  bits      = (uint8_t)(m->lengthNibbles * 4U);

        if (bits > 32) {
            fatal("relocateSicXE: modification length exceeds 32 bits");
        }

        uint64_t mask64 = (bits == 32) ? 0xFFFFFFFFULL : ((1ULL << bits) - 1ULL);
        uint32_t targetAddr = m->address + (uint32_t)R;
        uint32_t aggregate  = 0;

        for (uint8_t b = 0; b < byteCount; ++b) {
            aggregate = (aggregate << 8) | memReadByte(targetAddr + b);
        }

        uint32_t field = (uint32_t)((aggregate >> shift) & (uint32_t)mask64);

        if (m->sign == '+') {
            field += (uint32_t)R;
        }
        else if (m->sign == '-') {
            field -= (uint32_t)R;
        }
        else {
            fatal("relocateSicXE: invalid sign in modification record (expected '+' or '-')");
        }

        field &= (uint32_t)mask64;

        uint32_t preservedLow = (shift == 0) ? 0U : (aggregate & ((1U << shift) - 1U));
        uint32_t newValue = (field << shift) | preservedLow;

        for (int b = (int)byteCount - 1; b >= 0; --b) {
            uint8_t outByte = (uint8_t)(newValue & 0xFFU);
            memWriteByte(targetAddr + (uint32_t)b, outByte);
            newValue >>= 8;
        }
    }

    for (size_t i = 0; i < obj->textCount; i++) {
        textRecord *t = &obj->textRecords[i];
        uint32_t loadAddress = t->address + (uint32_t)R;

        for (uint32_t j = 0; j < t->length; j++) {
            t->bytes[j] = memReadByte(loadAddress + j);
        }

        t->address = loadAddress;
    }

    obj->header.startAddress       = reloc;
    obj->endRecord.firstExecAddress = (obj->endRecord.firstExecAddress + (uint32_t)R) & 0xFFFFFFu;
}
