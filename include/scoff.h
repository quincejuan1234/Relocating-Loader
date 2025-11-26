#ifndef SCOFF_H
#define SCOFF_H

/**
 * SCOFF record types for SIC and SIC/XE object files.
 * 
 * This module is responsible for defining the structures used for:
 * - Header record (H)
 * - Text record (T)
 * - Modification record (M)
 * - End record (E)
*/

#include <stdint.h>

typedef struct {
    char program_name[7]; 
    uint32_t startAddress;
    uint32_t programLength;
} headerRecord

typedef struct {
    uint32_t address;
    uint32_t length;
    uint8_t bytes[32];      // Max T-record length = 32 bytes
} textRecord;

typedef struct {
    uint32_t address;
    uint8_t lengthNibbles;
    char sign;              // '+' or '-'
} modRecord;

typedef struct {
    uint32_t firstExecAddress;
} endRecord;

typedef struct {
    header header;
    textRecord *textRecords;
    size_t textCount;

    modRecord *modRecords;
    size_t modCount;

    EndRecord endRecord;
} objFile;

int parseFile(const char *path, objFile *out);
void objFree(objFile *file);

#endif