#include "loader.h"
#include "objFile.h"
#include "relocSic.h"
#include "relocSicXE.h"
#include "util.h"
#include <stdio.h>

/** 
 * High-level control logic for the SIC / SICXE relocating loader.
 *
 * This file implements:
 *   - Implement runLoader(), the main function exposed by loader.h
 *   - Call objParseFile() to read the input object file into a objFile
 *   - Based on the MachineType (SIC or SICXE), call the appropriate
 *     relocation backend (relocateSic() or relocateSicXE())
 *   - After relocation, emit the relocated T (Text) and E (End) records
 *     to stdout in the expected object file format
 *   - Clean up any allocated resources (via objFree())
 *
 * This module is in charge of calling the other functions of the loader.
 */

static void printRelocatedRecords(const objFile *obj){
    for(size_t i = 0; i < obj->textCount; i++){
        const textRecord *t = &obj->textRecords[i];

        printf("T%06X%02X", ((unsigned int)t->address), ((unsigned int)t->length));
        for(size_t j = 0; j < t->length; j++){
            printf("%02X", ((unsigned int)t->bytes[j]));
        }//Iterate through the object code bytes
        printf("\n");
    }//Iterate through the Text records

    printf("E%06X\n", (unsigned int)obj->endRecord.firstExecAddress);
}

int runLoader(const LoaderConfig *config) {
    objFile obj = {0};

    if (objParseFile(config->filePath, &obj) != 0) {
        fatal("Failed to parse SCOFF file.");
    }

    if (config->machineType == MACHINE_SIC) {
        relocateSic(&obj, config->relocationAddress);
    } else {
        relocateSicXE(&obj, config->relocationAddress);
    }
    
    printRelocatedRecords(&obj);
    objFree(&obj);
    return 0;
}
