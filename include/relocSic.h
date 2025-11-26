#ifndef RELOC_SIC_H
#define RELOC_SIC_H

#include "scoff.h"
#include <stdint.h>

/**
 * Interface for relocation of classic SIC object programs.
 *
 * This header declares:
 *   - relocate_sic(), which applies a relocation factor to a ScoffFile
 *     containing SIC object code
 *
 * The implementation in reloc_sic.c:
 *   - Walks modification records
 *   - Adjusts addresses/words according to the relocation address
 *   - Ensures results respect SIC 24-bit addressing constraints
 */

void relocateSic(objFile *obj, uint32_t reloc);

#endif
