#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <bmp.h>

FILE *get_stream(char *path)
{
  FILE *stream = fopen(path, "r");
  if (stream == NULL)
  {
    fprintf(stderr, "Error opening bmp file with path: %s", path);
    exit(1);
  }
  return stream;
}

void load_header(FILE *stream, bmp_header *header)
{
  size_t result = fread((void *)header, 1, sizeof(bmp_header), stream);
  if (result != sizeof(bmp_header))
  {
    fprintf(stderr, "Error loading bmp header");
    exit(1);
  }
}

void skip_offset(FILE *stream, u_int32_t offset)
{
  size_t skip_size = offset - sizeof(bmp_header);
  char dump[skip_size];
  size_t result = fread((void *)dump, 1, skip_size, stream);
  if (result != skip_size)
  {
    fprintf(stderr, "Error skipping bmp offset");
    exit(1);
  }
}

void read_bmp_byte(FILE *stream, char *output)
{
  size_t result = fread((void *)output, 1, 1, stream);
  if (result == NULL)
  {
    fprintf(stderr, "Error reading byte from bmp file");
    exit(1);
  }
}

void close_stream(FILE *stream)
{
  int result = fclose(stream);
  if (result != 0)
  {
    fprintf(stderr, "Error close bmp stream");
    exit(1);
  }
}