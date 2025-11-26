#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>

int parseHex (const char *s, uint32_t *out);
void fatal(const char *msg);

#endif
