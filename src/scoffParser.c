#include "scoff.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Placeholder parser.

int parseFile(const char *path, objFile *out) {
    // TODO: implement SCOFF parser
    return 0;
}

void objFree(objFile *file) {
    free(file->textRecords);
    free(file->modRecords);
}
