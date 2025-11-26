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


int objParseFile(const char *path, objFile *out) {
    // TODO: implement SCOFF parser
    return 0;
}

void objFree(objFile *file) {
    free(file->textRecords);
    free(file->modRecords);
}
