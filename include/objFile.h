#ifndef OBJFILE_H
#define OBJFILE_H


/*
 * Definitions for the object file format used by the SIC/SICXE loader.
 *
 * This header declares:
 *   - Structs representing H (header), T (text), M (modification), and E (end) records
 *   - The objFile aggregate structure that holds all records
 *   - The parsing functions: parseFile() and objFree()
 *
 * The implementation in objParser.c:
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
    headerRecord header;
    textRecord *textRecords;
    size_t textCount;
    modRecord *modRecords;
    size_t modCount;
    endRecord endRecord;
} objFile;

int objParseFile(const char *path, objFile *out);
void objFree(objFile *file);

#endif