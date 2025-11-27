#ifndef RELOC_SIC_H
#define RELOC_SIC_H

#include "objFile.h"
#include <stdint.h>

/**
 * Interface for relocation of classic SIC object programs.
 *
 * This header declares:
 *   - relocateSic(), which applies a relocation factor to a objFile
 *     containing SIC object code
 *
 * The implementation in relocSic.c:
 *   - Walks modification records
 *   - Adjusts addresses/words according to the relocation address
 *   - Ensures results respect SIC 24-bit addressing constraints
 */

void relocateSic(objFile *obj, uint32_t reloc);

#endif
