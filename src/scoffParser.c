#include "scoff.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Implementation of the SCOFF object file parser for SIC/SICXE.
 *
 * Responsibilities:
 *   - Implement scoff_parse_file(), which:
 *       * Opens the given object file
 *       * Reads each line and identifies its record type (H/T/M/E)
 *       * Parses fields into ScoffHeader, ScoffTextRecord,
 *         ScoffModRecord, and ScoffEndRecord
 *       * Stores all records in a ScoffFile structure
 *       * Performs basic validation (record order, lengths, addresses)
 *   - Implement scoff_free(), which releases any dynamic memory
 *     allocated inside a ScoffFile
 *
 * This module should not perform relocation; it only parses input into
 * an in-memory representation suitable for later processing.
 */


int parseFile(const char *path, objFile *out) {
    // TODO: implement SCOFF parser
    return 0;
}

void objFree(objFile *file) {
    free(file->textRecords);
    free(file->modRecords);
}
