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
	uint8_t dump[skipSize];
	size_t result = fread((void *)dump, 1, skipSize, stream);
	if (result != skipSize) {
		fprintf(stderr, "Error skipping bmp offset");
		exit(EXIT_FAILURE);
	}
}

void copyBmpHeaderAndOffset(FILE *src, FILE *dest, BmpHeader *header) {
	size_t bytesRead = fread((void *)header, 1, sizeof(BmpHeader), src);

	if (bytesRead != sizeof(BmpHeader)) {
		fprintf(stderr, "Error reading bmp header");
		exit(EXIT_FAILURE);
	}

	fwrite((void *)header, 1, sizeof(BmpHeader), dest);

	size_t offsetSize = header->offset - sizeof(BmpHeader);
	uint8_t dump[offsetSize];

	bytesRead = fread((void *)dump, 1, offsetSize, src);

	if (bytesRead != offsetSize) {
		fprintf(stderr, "Error reading bmp offset");
		exit(EXIT_FAILURE);
	}

	fwrite((void *)dump, 1, offsetSize, dest);
}
