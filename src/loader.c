#include "loader.h"
#include "scoff.h"
#include "reloc_sic.h"
#include "reloc_sicxe.h"
#include <stdio.h>

/** 
 * High-level control logic for the SIC / SICXE relocating loader.
 *
 * Responsibilities:
 *   - Implement run_loader(), the main API exposed by loader.h
 *   - Call scoff_parse_file() to read the input object file into a ScoffFile
 *   - Based on the MachineType (SIC or SICXE), call the appropriate
 *     relocation backend (relocate_sic() or relocate_sicxe())
 *   - After relocation, emit the relocated T (Text) and E (End) records
 *     to stdout in the expected SCOFF-like format
 *   - Clean up any allocated resources (via scoff_free())
 *
 * This module is the orchestrator; it does not implement the detailed
 * parsing or relocation algorithms itself.
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