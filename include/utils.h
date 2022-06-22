#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdint.h>

void exitWithError(char *message);

int loadStream(FILE *stream, uint8_t *dest, size_t bytes);
size_t saveStream(FILE *stream, uint8_t *src, size_t bytes);
#endif