#include <bmp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *getStream(char *path) {
	FILE *stream = fopen(path, "r");
	if (stream == NULL) {
		fprintf(stderr, "Error opening bmp file with path: %s", path);
		exit(EXIT_FAILURE);
	}
	return stream;
}

void loadHeader(FILE *stream, bmpHeader *header) {
	size_t result = fread((void *)header, 1, sizeof(bmpHeader), stream);
	if (result != sizeof(bmpHeader)) {
		fprintf(stderr, "Error loading bmp header");
		exit(EXIT_FAILURE);
	}
}

void skipOffset(FILE *stream, u_int32_t offset) {
	size_t skipSize = offset - sizeof(bmpHeader);
	char dump[skipSize];
	size_t result = fread((void *)dump, 1, skipSize, stream);
	if (result != skipSize) {
		fprintf(stderr, "Error skipping bmp offset");
		exit(EXIT_FAILURE);
	}
}

void readBmpByte(FILE *stream, char *output) {
	size_t result = fread((void *)output, 1, 1, stream);
	if (result == NULL) {
		fprintf(stderr, "Error reading byte from bmp file");
		exit(EXIT_FAILURE);
	}
}

void closeStream(FILE *stream) {
	int result = fclose(stream);
	if (result != 0) {
		fprintf(stderr, "Error close bmp stream");
		exit(EXIT_FAILURE);
	}
}