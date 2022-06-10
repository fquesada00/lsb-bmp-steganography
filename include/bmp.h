#ifndef __BMP_H__
#define __BMP_H__
#include <stdlib.h>
#include <stdbool.h>
/*
 * source: https://docs.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-bitmapfileheader?redirectedfrom=MSDN
 */
#pragma pack(1)
typedef struct
{
  u_int16_t type;
  u_int32_t size;
  u_int16_t reserved1;
  u_int16_t reserved2;
  u_int32_t offset; // starting address in bytes (from beginning)
} bmp_header;

FILE *get_stream(char *path);

void load_header(FILE *stream, bmp_header *header);

bool read_bmp(FILE *stream, char *output, u_int64_t size);

void close_stream(FILE *stream);

void load_header(FILE *stream, bmp_header *header);

void skip_offset(FILE *stream, u_int32_t offset);

#endif