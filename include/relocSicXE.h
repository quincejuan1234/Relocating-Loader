#ifndef RELOC_SICXE_H
#define RELOC_SICXE_H

#include "scoff.h"
#include <stdint.h>

/**
 * Interface for relocation of SIC/XE object programs.
 *
 * This header declares:
 *   - relocateSicXE(), which applies relocation to a objFile
 *     containing SIC/XE object code
 *
 * The implementation in relocSicXE.c:
 *   - Interprets modification records for SIC/XE
 *   - Adjusts the appropriate fields for formats 3/4 (and others as needed)
 *   - Respects SIC/XE addressing rules and field sizes
 */

void relocateSicXE(objFile *obj, uint32_t reloc);

#endif
