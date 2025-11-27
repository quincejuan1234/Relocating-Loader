#include "relocSic.h"

/**
 * Relocation logic for classic SIC object programs.
 *
 * Responsibilities:
 *   - Implement relocateSic(), which:
 *       * Receives a parsed objFile and a relocation address
 *       * Computes the relocation factor (newStart - originalStart)
 *       * Walks all SIC modification records (M)
 *       * Reads the affected word(s) and adds the relocation factor
 *       * Ensures the relocated values fit within SIC 24-bit addresses
 *   - Update the header/end records or any other fields as required
 *     by the project spec for relocated programs
 *
 * This module assumes the object file contents are already parsed by
 * scoff_parser.c and focuses only on adjusting addresses for SIC.
 */


void relocateSic(obj *obj, uint32_t reloc) {
    // TODO: implement SIC relocation
}
