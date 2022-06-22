#include <bmp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void loadHeader(FILE *stream, BmpHeader *header) {
	size_t result = fread((void *)header, 1, sizeof(BmpHeader), stream);
	if (result != sizeof(BmpHeader)) {
		fprintf(stderr, "Error loading bmp header");
		exit(EXIT_FAILURE);
	}
}

void skipOffset(FILE *stream, uint32_t offset) {
	size_t skipSize = offset - sizeof(BmpHeader);
	char dump[skipSize];
	size_t result = fread((void *)dump, 1, skipSize, stream);
	if (result != skipSize) {
		fprintf(stderr, "Error skipping bmp offset");
		exit(EXIT_FAILURE);
	}
}

uint8_t readBmpByte(FILE *stream) {
	uint8_t byte;
	fread(&byte, 1, 1, stream);
	if (ferror(stream) != 0) {
		fprintf(stderr, "Error reading byte from bmp file");
		exit(EXIT_FAILURE);
	}
	if (feof(stream) != 0) {
		fprintf(stderr, "Reached EOF reading byte from bmp file");
		exit(EXIT_FAILURE);
	}
	return byte;
}