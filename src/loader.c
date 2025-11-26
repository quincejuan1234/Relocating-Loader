#include "loader.h"
#include "scoff.h"
#include "reloc_sic.h"
#include "reloc_sicxe.h"
#include <stdio.h>

/**
 * High-level loader pipeline:
 * 1. Parse SCOFF file
 * 2. Run SIC or SIC/XE relocation
 * 3. Print relocated T and E records
 * */ 

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