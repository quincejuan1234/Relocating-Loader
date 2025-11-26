#ifndef SCOFF_H
#define SCOFF_H


/*
 * Definitions for the SCOFF-style object file format used by the SIC/SICXE loader.
 *
 * This header declares:
 *   - Structs representing H (header), T (text), M (modification),
 *     and E (end) records
 *   - The ScoffFile aggregate structure that holds all parsed records
 *   - The parsing API: scoff_parse_file() and scoff_free()
 *
 * The implementation in scoff_parser.c:
 *   - Reads a textual SIC/SICXE object file line by line
 *   - Parses each H/T/M/E record into the corresponding C structs
 *   - Performs basic validation on lengths, addresses, and record ordering
 */

#include <stdint.h>

typedef struct {
    char program_name[7]; 
    uint32_t startAddress;
    uint32_t programLength;
} headerRecord;

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

    endRecord endRecord;
} objFile;

int parseFile(const char *path, objFile *out);
void objFree(objFile *file);

#endif