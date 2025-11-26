#include <stdio.h>
#include <string.h>
#include "loader.h"
#include "util.h"

// Parses CLI args and invokes run_loader()

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("ERROR: Usage: %s <object_file> <reloc_address_hex> <SIC|SICXE>\n", argv[0]);
        return 1;
    }

    LoaderConfig config;
    config.file_path = argv[1];

    if (!parseHex (argv[2], &config.relocation_address)) {
        fatal("Invalid hex relocation address.");
    }

    if (strcmp(argv[3], "SIC") == 0) {
        config.machine_type = MACHINE_SIC;
    } else if (strcmp(argv[3], "SICXE") == 0) {
        config.machine_type = MACHINE_SICXE;
    } else {
        fatal("Invalid machine type. Use SIC or SICXE.");
    }

    return run_loader(&config);
}
