#ifndef __UTILS_H__
#define __UTILS_H__

#include <constants.h>
#include <stdint.h>
#include <stdio.h>

#define BIT_MASK(__TYPE__, __ONE_COUNT__)                                                                                        \
	((__TYPE__)(-((__ONE_COUNT__) != 0))) & (((__TYPE__)-1) >> ((sizeof(__TYPE__) * BYTE_BITS) - (__ONE_COUNT__)))

void exitWithError(char *message);

FILE *createStream(char *path, const char *mode);

int loadStream(FILE *stream, uint8_t *dest, size_t bytes);

size_t saveStream(FILE *stream, uint8_t *src, size_t bytes);

void closeStream(FILE *stream);

FILE *saveExtractedMessageToFile(uint8_t *extractedMessage, uint32_t length, char *outputFileName);

uint32_t getFileLength(FILE *stream);

uint8_t readByte(FILE *stream);

void writeByte(FILE *stream, uint8_t byte);

FILE *copyEncodedInputToFile(FILE *inputStream, char *extension);

#endif
