#include "objFile.h"


/**
 * Implementation of the object file parser for SIC/SICXE.
 *
 * This file implements:
 *   - Implement objParseFile(), which:
 *       * Opens the given object file
 *       * Reads each line and identifies its record type (H/T/M/E)
 *       * Parses fields into headerRecord, textRecord,
 *         ModRecord, and EndRecord
 *       * Stores all records in a objFile structure
 *       * Performs basic validation (record order, lengths, addresses)
 *   - Implement objFree(), which releases any dynamic memory
 *     allocated inside a objFile
 *
 * This file only parses input into an in-memory representation 
 * suitable for later processing.
 */

// Remove trailing '\n' and '\r' characters
static void trimEolChars(char *s)
{
    size_t len = strlen(s);
    while (len > 0 && (s[len - 1] == '\n' || s[len - 1] == '\r')) {
        s[--len] = '\0';
    }
}

// Parse exactly 'len' hex characters at p into a uint32_t. Returns 1 on success, 0 on failure.

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
    modRecord  *mRecords = NULL; // Array that holds the M records
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

    while (!error && fgets(line, sizeof(line), fp) != NULL){
        lineNum++;
        trimEolChars(line);

        //Skip empty lines
        int spaceLine = 1;
        for (char *p = line; *p; ++p) {
            if (!isspace((unsigned char)*p)) {
                spaceLine = 0;// found non-space char
                break;
            }
        }
        if(spaceLine) {
            continue;// Continue to next line
        }

        char recType = line[0];// Character with the record type of the line
        const char *fields = line + 1;
        // Skip any spaces immediately after the record type. Some object files
        // include a separating space.
        while (*fields == ' ' || *fields == '\t') {
            ++fields;
        }
        switch (recType)
        {
        // Read in header record
        case 'H':{
            // Header must be first and unique
            if (seenHRecord || tCount > 0 || mCount > 0 || seenERecord){
                error = 1;
                break;
            }
            // Some assemblers pad the program name to 6 chars with spaces,
            // while others emit a shorter name followed by a single space
            // before the start and length fields. Parse the name up to the
            // next whitespace (max 6 chars) and then read the two required
            // 6-digit hex fields with optional spacing in between.

            const char *p = fields;
            char sicProgName[7];
            size_t nameLen = 0;

            while (*p && !isspace((unsigned char)*p) && nameLen < sizeof(sicProgName) - 1) {
                sicProgName[nameLen++] = *p++;
            }
            sicProgName[nameLen] = '\0';

            // Skip whitespace between name and start address
            while (isspace((unsigned char)*p)) {
                ++p;
            }

            // Need at least 6 hex chars for the start address
            if (strlen(p) < 6 || !parseFixedHex(p, 6, &progStart)) {
                error = 1;
                break;
            }
            p += 6;

            // Skip whitespace between start address and program length
            while (isspace((unsigned char)*p)) {
                ++p;
            }

            // Need at least 6 hex chars for the program length
            if (strlen(p) < 6 || !parseFixedHex(p, 6, &headerLen)) {
                error = 1;
                break;
            }
            seenHRecord = 1;

            // Fill header record in objfile struct
            memset(&out->header, 0, sizeof(out->header));
            strncpy(out->header.progName, sicProgName, sizeof(out->header.progName) - 1);
            out->header.startAddress  = progStart;
            out->header.programLength = headerLen;

            break;
        }
            
        // Read in text record
        case 'T':{
            // detects if T record is before H record or after E record
            if (!seenHRecord || seenERecord) {
                error = 1;
                break;
            }

            size_t lineLen = strlen(fields);
            // Minimun size of T record payload: 6 addr + 2 len = 8 chars
            if (lineLen < 8) {
                error = 1;
                break;
            }

            uint32_t addr = 0;
            uint32_t tLen = 0;

            // Cols 2–7: address (6 hex), 8–9: length (2 hex)
            if (!parseFixedHex(fields, 6, &addr) || !parseFixedHex(fields + 6, 2, &tLen)) {
                error = 1;
                break;
            }

            // check T length has a valid lenght
            if (tLen > MAX_T_BYTES) {
                error = 1;
                break;
            }

            const char *hexBytes = fields + 8; // String with the hex bytes of the T record
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
            tr->length = tLen;

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
            if (!seenHRecord || seenERecord) {
                // M before H or after E
                error = 1;
                break;
            }

            size_t len = strlen(fields);
            // Minimun size of M record payload: 6 addr + 2 len + 1 sign = 9 chars.
            if (len < 9) {
                error = 1;
                break;
            }

            uint32_t mAddr = 0; // Address where the loader must apply the relocation
            uint32_t nibbles  = 0; // length of the field to modify, in nibbles

            // Checks that the address and the number of nibbles are in the correct format 
            if (!parseFixedHex(fields, 6, &mAddr) || !parseFixedHex(fields + 6, 2, &nibbles)) {
                error = 1;
                break;
            }

            // Check for correct characters (+ , -)
            char sign = fields[8];
            if (sign != '+' && sign != '-') {
                error = 1;
                break;
            }

            // Verifies that the lengthNibbles should be > 0 
            if (nibbles == 0 || nibbles > 0xFFU) {
                error = 1;
                break;
            }

            // Grow modification records array if needed 
            if (mCount == mCapacity) {
                if (mCapacity == 0){
                   mCapacity = 8;
                }
                else{
                    mCapacity *= 2;
                }
                modRecord *temp = (modRecord *)realloc(mRecords, mCapacity * sizeof(modRecord));
                if (!temp) {
                    error = 1;
                    break;
                }
                mRecords = temp;
            }

            modRecord *mr = &mRecords[mCount];
            memset(mr, 0, sizeof(*mr));
            mr->address = mAddr;
            mr->lengthNibbles = (uint8_t)nibbles;
            mr->sign = sign;

            // Check mod record address vs. program length
            if (headerLen > 0) {
                uint32_t headerEnd = progStart + headerLen;
                if (mAddr < progStart || mAddr >= headerEnd) {
                    // Modification outside program range
                    error = 1;
                    break;
                }
            }

            mCount++;
            break;
        }

        // Read in end record
        case 'E':{
            if (!seenHRecord || seenERecord) {
                // E before H or multiple E records
                error = 1;
                break;
            }

            size_t lineLen = strlen(fields);
            // E record payload: 6 address chars
            if (lineLen < 6) {
                error = 1;
                break;
            }

            uint32_t execAddr = 0;
            if (!parseFixedHex(fields, 6, &execAddr)) {
                error = 1;
                break;
            }

            // Basic check: if headerLen > 0, entry point should be in range
            if (headerLen > 0) {
                uint32_t headerEnd = progStart + headerLen;
                if (execAddr < progStart || execAddr >= headerEnd) {
                    // Invalid program addresses
                    error = 1;
                    break;
                }
            }

            out->endRecord.firstExecAddress = execAddr;
            seenERecord = 1;
            break;
        }  

        default:
            // Invalid record
            error = 1;
            break;
        }// end switch
    }// end while
    
    // Error validation
    if (!error){
        // Check that the program has exactly one header and one end record
        if (!seenHRecord || !seenERecord) {
            error = 1;
        }
    }
    
    if (!error && tCount > 0 && headerLen > 0) {
        // If text records are present and non-zero program length, check the overall range
        uint32_t headerEnd = progStart + headerLen;
        if (minTextAddr < progStart || maxTextAddr > headerEnd) {
            error = 1;
        }
    }

    if (error) {
        // On failure, free the records arrays and reset out
        free(tRecords);
        free(mRecords);
        memset(out, 0, sizeof(*out));
        fclose(fp);
        return -1;
    }

    // If no errors, build the output objFile
    out->textRecords = tRecords;
    out->textCount = tCount;
    out->modRecords = mRecords;
    out->modCount = mCount;

    fclose(fp);
    return 0;
}

void objFree(objFile *file) {
    if (!file) {
        return;
    }

    // free records arrays
    free(file->textRecords);
    free(file->modRecords);

    // Reset pointers and counts
    file->textRecords = NULL;
    file->modRecords  = NULL;
    file->textCount   = 0;
    file->modCount    = 0;

    //clear header and endRecord
    memset(&file->header,    0, sizeof(file->header));
    memset(&file->endRecord, 0, sizeof(file->endRecord));
}