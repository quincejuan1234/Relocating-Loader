#include "loader.h"
#include "objFile.h"
#include "relocSic.h"
#include "relocSicXE.h"
#include <stdio.h>

/** 
 * High-level control logic for the SIC / SICXE relocating loader.
 *
 * Responsibilities:
 *   - Implement runLoader(), the main function exposed by loader.h
 *   - Call parseFile() to read the input object file into a objFile
 *   - Based on the MachineType (SIC or SICXE), call the appropriate
 *     relocation backend (relocateSic() or relocateSicXE())
 *   - After relocation, emit the relocated T (Text) and E (End) records
 *     to stdout in the expected object file format
 *   - Clean up any allocated resources (via objFree())
 *
 * This module is in charge of calling the other functions of the loader
 * it does not implement the detailed parsing or relocation algorithms itself.
 */


int runLoader(const LoaderConfig *config) {
    objFile obj = {0};

    if (parseFile(config->filePath, &obj) != 0) {
        fatal("Failed to parse SCOFF file.");
    }

    if (config->machineType == MACHINE_SIC) {
        relocateSic(&obj, config->relocationAddress);
    } else {
        relocateSicXE(&obj, config->relocationAddress);
    }

    // TODO: print relocated T and E records

    objFree(&obj);
    return 0;
}