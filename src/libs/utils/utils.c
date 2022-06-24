#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void exitWithError(char *message) {
	perror(message);
	exit(EXIT_FAILURE);
}

FILE *createStream(char *path, const char *mode) {
	printf("Opening file %s\n", path);
	FILE *stream = fopen(path, mode);
	if (stream == NULL) {
		fprintf(stderr, "Error opening bmp file with path: %s\n", path);
		exit(EXIT_FAILURE);
	}
	return stream;
}

size_t saveStream(FILE *stream, uint8_t *src, size_t bytes) {
	size_t bytesWritten = fwrite((void *)src, 1, bytes, stream);
	return bytesWritten;
}

void closeStream(FILE *stream) {
	int result = fclose(stream);
	if (result != 0) {
		fprintf(stderr, "Error close bmp stream");
		exit(EXIT_FAILURE);
	}
}

void saveExtractedMessageToFile(uint8_t *fileData, uint32_t fileLength, uint8_t *fileExtension, char *outputFileName) {
	// create new string with file name and extension

	char *fileName = malloc(strlen(outputFileName) + strlen((char *)fileExtension) + 1);
	strcpy(fileName, outputFileName);
	strcat(fileName, (char *)fileExtension);

	FILE *outStream = createStream(fileName, "w");

	saveStream(outStream, fileData, fileLength);

	closeStream(outStream);
	free(fileName);
}

uint32_t getFileLength(FILE *stream) {
	// TODO: atajar errores
	fseek(stream, 0L, SEEK_END);
	long imageSize = ftell(stream);
	rewind(stream);
	return (uint32_t)imageSize;
}

uint8_t readByte(FILE *stream) {
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

void writeByte(FILE *stream, uint8_t byte) {
	fwrite(&byte, 1, 1, stream);
	if (ferror(stream) != 0) {
		fprintf(stderr, "Error writing byte to bmp file");
		exit(EXIT_FAILURE);
	}
}
