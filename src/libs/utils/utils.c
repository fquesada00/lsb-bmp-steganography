#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void exitWithError(char *message) {
	perror(message);
	exit(EXIT_FAILURE);
}

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

int loadStream(FILE *stream, uint8_t *dest, size_t bytes) {
	size_t bytesRead = fread((void *)dest, 1, bytes, stream);
	// check if EOF is reached
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

void saveExtractedMessageToFile(u_int8_t *fileData, u_int32_t fileLength, u_int8_t *fileExtension, char *outputFileName) {
	// create new string with file name and extension

	char *fileName = malloc(strlen(outputFileName) + strlen((char *)fileExtension) + 1);
	strcpy(fileName, outputFileName);
	strcat(fileName, (char *)fileExtension);

	FILE *outStream = createStream(fileName);

	saveStream(outStream, fileData, fileLength);

	closeStream(outStream);
	free(fileName);
}