#ifndef LOADER_H
#define LOADER_H

#include <stdint.h>
#include "scoff.h"

// Machine type enum
typedef enum {
    MACHINE_SIC = 0,
    MACHINE_SICXE = 1
} machineType;

// Loader configuration passed from main.c
typedef struct {
    const char *filePath;
    uint32_t relocationAddress;
    MachineType machineType;
} LoaderConfig;

// Main loader entry point
int runLoader(const LoaderConfig *config);

#endif