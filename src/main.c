#include <stdio.h>
#include <string.h>
#include "loader.h"
#include "util.h"

/**
 * Entry point for the SIC / SICXE relocating loader.
 *
 * This file implements:
 *   - Parse and validate command-line arguments:
 *       <objectFile> <relocAddressHex> <SIC|SICXE>
 *   - Convert the relocation address from a hex string to an integer
 *   - Map the machine type string to the MachineType enum
 *   - Populate a LoaderConfig and call runLoader()
 *
 * This file processes the console input into the loader defined in loader.h.
 */


int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("ERROR: Usage: %s <objectFile> <relocAddressHex> <SIC|SICXE>\n", argv[0]);
        return 1;
    }

    LoaderConfig config;
    config.filePath = argv[1];

    if (!parseHex (argv[2], &config.relocationAddress)) {
        fatal("Invalid hex relocation address.");
    }

    if (strcmp(argv[3], "SIC") == 0) {
        config.machineType = MACHINE_SIC;
    } 
    else if (strcmp(argv[3], "SICXE") == 0) {
        config.machineType = MACHINE_SICXE;
    } 
    else {
        fatal("Invalid machine type. Use SIC or SICXE.");
    }

    return run_loader(&config);
}
