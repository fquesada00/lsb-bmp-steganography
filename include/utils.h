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

void saveExtractedMessageToFile(uint8_t *fileData, uint32_t fileLength, uint8_t *fileExtension, char *outputFileName);

uint32_t getFileLength(FILE *stream);

uint8_t readByte(FILE *stream);

void writeByte(FILE *stream, uint8_t byte);

#endif