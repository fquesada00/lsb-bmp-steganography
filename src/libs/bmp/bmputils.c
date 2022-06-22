#include <bmp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *getStream(char *path) {
	printf("Opening file %s\n", path);
	FILE *stream = fopen(path, "r");
	if (stream == NULL) {
		fprintf(stderr, "Error opening bmp file with path: %s\n", path);
		exit(EXIT_FAILURE);
	}
	return stream;
}

FILE *createStream(char *name) {
	printf("Creating file %s\n", name);
	FILE *stream = fopen(name, "w");
	if (stream == NULL) {
		fprintf(stderr, "Error creating file with name: %s\n", name);
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

void skipOffset(FILE *stream, uint32_t offset) {
	size_t skipSize = offset - sizeof(bmpHeader);
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
	if(feof(stream) != 0) {
		fprintf(stderr, "Reached EOF reading byte from bmp file");
		exit(EXIT_FAILURE);
	}
	return byte;
}

void closeStream(FILE *stream) {
	int result = fclose(stream);
	if (result != 0) {
		fprintf(stderr, "Error close bmp stream");
		exit(EXIT_FAILURE);
	}
}