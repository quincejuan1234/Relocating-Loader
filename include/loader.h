#ifndef LOADER_H
#define LOADER_H

#include <stdint.h>
#include "objFile.h"

/*
 * High-level interface for the SIC / SICXE relocating loader.
 *
 * This header defines:
 *   - The MachineType enum (SIC vs SICXE)
 *   - The LoaderConfig struct, which contains the command-line configurations
 *   - The runLoader() API, which drives the whole loading/relocation pipeline
 *
 * The implementation in loader.c:
 *   - Parses a objFile using objParser.c
 *   - Invokes the appropriate relocation backend funcitons relocSic.c or 
 *     relocSicxe.c depending on the case
 *   - Emits relocated T and E records to stdout
 */


// Machine type enum
typedef enum {
    MACHINE_SIC = 0,
    MACHINE_SICXE = 1
} machineType;

// Loader configuration passed from main.c
typedef struct {
    const char *filePath;
    uint32_t relocationAddress;
    machineType machineType;
} LoaderConfig;

// Main loader entry point
int runLoader(const LoaderConfig *config);

#endif
