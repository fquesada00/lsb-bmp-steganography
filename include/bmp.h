#ifndef __BMP_H__
#define __BMP_H__
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * source: https://docs.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-bitmapfileheader?redirectedfrom=MSDN
 */
#pragma pack(push, 1)
typedef struct {
	uint16_t type;
	uint32_t size;
	uint16_t reserved1;
	uint16_t reserved2;
	uint32_t offset; // starting address in bytes (from beginning)
} BmpHeader;

#pragma pack(pop)

void loadHeader(FILE *stream, BmpHeader *header);

void skipOffset(FILE *stream, uint32_t offset);

void copyBmpHeaderAndOffset(FILE *src, FILE *dest, BmpHeader *header);

#endif
