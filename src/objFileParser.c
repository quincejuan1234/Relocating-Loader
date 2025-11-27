#include "objFile.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Implementation of the object file parser for SIC/SICXE.
 *
 * Responsibilities:
 *   - Implement objParseFile(), which:
 *       * Opens the given object file
 *       * Reads each line and identifies its record type (H/T/M/E)
 *       * Parses fields into ScoffHeader, ScoffTextRecord,
 *         ModRecord, and EndRecord
 *       * Stores all records in a objFile structure
 *       * Performs basic validation (record order, lengths, addresses)
 *   - Implement objFree(), which releases any dynamic memory
 *     allocated inside a objFile
 *
 * This module should not perform relocation; it only parses input into
 * an in-memory representation suitable for later processing.
 */

// Remove trailing '\n' and '\r' characters
static void trimEolChars(char *s)
{
    size_t len = strlen(s);
    while (len > 0 && (s[len - 1] == '\n' || s[len - 1] == '\r')) {
        s[--len] = '\0';
    }
}

/* Parse exactly 'len' hex characters at p into a uint32_t.
 * Returns 1 on success, 0 on failure.
 */
static int parseFixedHex(const char *p, size_t len, uint32_t *out){
    char buf[16]; // plenty for up to 8 hex digits and '\0' 
    char *end;
    unsigned long val;

    if (len == 0 || len >= sizeof(buf)) {
        return 0;
    }

    memcpy(buf, p, len);
    buf[len] = '\0';

    val = strtoul(buf, &end, 16);
    if (end != buf + len) {
        return 0; // non-hex garbage in field 
    }

    if (val > 0xFFFFFFFFUL) {
        return 0; // val does not fit in 32 bits
    }

    if (out) {
        *out = (uint32_t)val;
    }
    return 1;
}

int objParseFile(const char *path, objFile *out) {
    FILE *fp = NULL;
    char line[256]; //Line being read from the object code file
    textRecord *tRecords = NULL; // Array that holds the T records
    modRecord  *mrecords = NULL; // Array that holds the M records
    size_t tCapacity = 0, mCapacity = 0; // Capacity counters for the records arrays
    size_t tCount = 0, mCount = 0; // Number of parsed records
    int seenHRecord = 0; // flag indicating whether header record found
    int seenERecord = 0; // flag indicating whether end record found
    uint32_t progStart = 0; // Program starting address 
    uint32_t headerLen = 0; // Program length in bytes
    // For program length check
    uint32_t minTextAddr = 0xFFFFFFFFU; // Address of the first text record
    uint32_t maxTextAddr = 0; // Address of the last byte of the last text record

    if(!path || !out){
        return -1;
    }

    fp = fopen(path, "r");
    if (!fp) {
        return -1;
    }

    memset(out, 0, sizeof(*out));// initializes the output object file struct

    int error = 0; // Flag for succesfull parsing process (Assume succeed until failure)
    int lineNum = 0; // Line number being read for the object file

    while (fgets(line, sizeof(line), fp) != NULL){
        lineNum++;
        trimEolChars(line);

        //Skip empty lines
        int spaceLine = 0;
        for (char *p = line; *p; ++p) {
            if (!isspace((unsigned char)*p)) {
                spaceLine = 0;
                break;
            }
        }
        if(spaceLine) {
            continue;
        }

        char recType = line[0];// Character with the record type of the line
        switch (recType)
        {
        // Read in header record
        case 'H':{
            // Header must be first and unique
            if (seenHRecord || tCount > 0 || mCount > 0 || seenERecord){
                error = 1;
                break;
            }
            size_t lineLen = strlen(line);
            // H + 6 name + 6 start + 6 length = 19 chars min
            if (lineLen < 19) {
                error = 1;
                break;
            }

            // Read program name (cols 2-7)
            char sicProgName[7];
            memcpy(sicProgName, line + 1, 6);
            sicProgName[6] = '\0';

            // Trim any whitespace inside the program name
            for (int i = 5; i >= 0; --i) {
                if (sicProgName[i] == ' ') {
                    sicProgName[i] = '\0';
                } else {
                    break;
                }
            }

            if (!parseFixedHex(line + 7, 6, &progStart) || !parseFixedHex(line + 13, 6, &headerLen)) {
                error = 1;
                break;
            }
            seenHRecord = 1;

            // Fill header record in objfile struct
            memset(&out->header, 0, sizeof(out->header));
            strncpy(out->header.progName, sicProgName, sizeof(out->header.progName) - 1);
            break;
        }
            
        // Read in text record
        case 'T':{
            // detects if T record is before H record or after E record
            if (!seenHRecord || seenERecord) {
                error = 1;
                break;
            }

            size_t lineLen = strlen(line);
            // Minimun size of T record: At least 'T' + 6 addr + 2 len = 9 chars 
            if (lineLen < 9) {
                error = 1;
                break;
            }

            uint32_t addr = 0;
            uint32_t tLen = 0;

            // Cols 2–7: address (6 hex), 8–9: length (2 hex)
            if (!parseFixedHex(line + 1, 6, &addr) || !parseFixedHex(line + 7, 2, &tLen)) {
                error = 1;
                break;
            }

            // check T length has a valid lenght
            if (tLen > MAX_T_BYTES) {
                error = 1;
                break;
            }

            const char *hexBytes = line + 9; // String with the hex bytes of the T record
            size_t hexLen = strlen(hexBytes);// How many hex digits appear after the length field
            // Check if the length of the hex bytes is correct 
            if (hexLen != (size_t)tLen * 2U) {
                error = 1;
                break;
            }

            // Basic range check if header length is non-zero
            if (headerLen > 0) {
                uint32_t progEnd = progStart + headerLen;// Calculates the program end address
                uint32_t tEnd = addr + tLen; // Calculates the T record end address
                // Checks if the Text  record is outside declared program range
                if (addr < progStart || tEnd > progEnd) {
                    error = 1;
                    break;
                }
            }

            // Grow text records array if needed 
            if (tCount == tCapacity) {
                if (tCapacity == 0) {
                    tCapacity = 8;
                } 
                else {
                    tCapacity *= 2;
                }
                textRecord *temp = (textRecord *)realloc( tRecords, tCapacity * sizeof(textRecord));
                if (!temp) {
                    error = 1;
                    break;
                }
                tRecords = temp;
            }

            textRecord *tr = &tRecords[tCount];
            memset(tr, 0, sizeof(*tr));
            tr->address = addr;
            tr->length  = tLen;

            // Loop that Decodes hex string into bytes
            for (uint32_t i = 0; i < tLen; ++i) {
                char byteStr[3]; // Stores one byte(2 hex chars)
                byteStr[0] = hexBytes[i * 2];
                byteStr[1] = hexBytes[i * 2 + 1];
                byteStr[2] = '\0';

                // Converts the 2-char hex string into a numeric value
                uint32_t byteVal = 0;
                if (!parseFixedHex(byteStr, 2, &byteVal)) {
                    error = 1;
                    break;
                }
                tr->bytes[i] = (uint8_t)byteVal;// Stores the resulting byte in the text record struct
            }

            if (error) {
                break;
            }

            // Gets the min and max T record address of the program 
            if (addr < minTextAddr) {
                minTextAddr = addr;
            }
            if (addr + tLen > maxTextAddr) {
                maxTextAddr = addr + tLen;
            }

            tCount++;
            break;
        }
         
        // Read in modification record
        case 'M':{
            break;
        }

        // Read in end record
        case 'E':{
            break;
        }    

        default:
            break;
        }
        
    }
    
    return 0;
}

void objFree(objFile *file) {
    free(file->textRecords);
    free(file->modRecords);
}