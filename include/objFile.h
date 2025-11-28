#ifndef OBJFILE_H
#define OBJFILE_H


/*
 * Definitions for the object file format used by the SIC/SICXE loader.
 *
 * This header declares:
 *   - Structs representing H (header), T (text), M (modification), and E (end) records
 *   - The objFile aggregate structure that holds all records
 *   - The parsing functions: objParseFile() and objFree()
 *
 * The implementation in objFileParser.c:
 *   - Reads a textual SIC/SICXE object file line by line
 *   - Parses each H/T/M/E record into the corresponding C structs
 *   - Performs basic validation on lengths, addresses, and record ordering
 *   - Releases any dynamic memory allocated inside a objFile
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_T_BYTES 32 // Max T-record length

// Represents the header record
typedef struct {
    char progName[7]; // Program name
    uint32_t startAddress; // Starting load address of the program
    uint32_t programLength; // Total program length in bytes
} headerRecord;

// Represents the text record
typedef struct {
    uint32_t address; // Starting address where this text record should be loaded
    uint32_t length; // Number of bytes contained in this text record
    uint8_t bytes[MAX_T_BYTES]; // Object code bytes
} textRecord;

// Represents the modification record
typedef struct {
    uint32_t address; // Address of the field to modify
    uint8_t lengthNibbles; // Number of nibbles to modify
    char sign; // Relocation operator: '+' to add, '-' to subtract
} modRecord;

// Represents the end record
typedef struct {
    uint32_t firstExecAddress; // Starting load address of the program
} endRecord;

// struct representation of a SIC/SICXE object file.
typedef struct {
    headerRecord header; // Parsed H record information
    textRecord *textRecords; // Array of T records
    size_t textCount; // Number of text records parsed
    modRecord *modRecords; // Array of M records
    size_t modCount; // Number of modification records parsed
    endRecord endRecord; // E record information
} objFile;

int objParseFile(const char *path, objFile *out);
void objFree(objFile *file);

#endif