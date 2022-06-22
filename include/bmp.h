#ifndef __BMP_H__
#define __BMP_H__
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * source: https://docs.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-bitmapfileheader?redirectedfrom=MSDN
 */
#pragma pack(1)
typedef struct {
	u_int16_t type;
	u_int32_t size;
	u_int16_t reserved1;
	u_int16_t reserved2;
	u_int32_t offset; // starting address in bytes (from beginning)
} bmpHeader;

FILE *getStream(char *path);

void loadHeader(FILE *stream, bmpHeader *header);

void readBmpByte(FILE *stream, char *output);

void closeStream(FILE *stream);

void loadHeader(FILE *stream, bmpHeader *header);

void skipOffset(FILE *stream, u_int32_t offset);

#endif
