#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdint.h>
#include <stdio.h>

void exitWithError(char *message);

FILE *getStream(char *path);

FILE *createStream(char *name);

int loadStream(FILE *stream, uint8_t *dest, size_t bytes);

size_t saveStream(FILE *stream, uint8_t *src, size_t bytes);

void closeStream(FILE *stream);

void saveExtractedMessageToFile(u_int8_t *fileData, u_int32_t fileLength, u_int8_t *fileExtension, char *outputFileName);

#endif