#include <constants.h>
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
		perror("ERROR");
		fprintf(stderr, "Error opening file with path: %s\n", path);
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

FILE *saveExtractedMessageToFile(uint8_t *fileData, uint32_t fileLength, uint8_t *fileExtension, char *outputFileName) {
	// create new string with file name and extension

	char *fileName = malloc(strlen(outputFileName) + strlen((char *)fileExtension) + 1);
	strcpy(fileName, outputFileName);
	strcat(fileName, (char *)fileExtension);

	FILE *outStream = createStream(fileName, "w");

	saveStream(outStream, fileData, fileLength);

	free(fileName);

	return outStream;
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
		fprintf(stderr, "Error reading byte from file");
		exit(EXIT_FAILURE);
	}
	if (feof(stream) != 0) {
		fprintf(stderr, "Reached EOF reading byte from file");
		exit(EXIT_FAILURE);
	}

	return byte;
}

void writeByte(FILE *stream, uint8_t byte) {
	fwrite(&byte, 1, 1, stream);
	if (ferror(stream) != 0) {
		fprintf(stderr, "Error writing byte to file");
		exit(EXIT_FAILURE);
	}
}

FILE *copyEncodedInputToFile(FILE *inputStream, char *extension) {
	uint32_t inputLength = getFileLength(inputStream);

	FILE *tmp = createStream(TMP_FILENAME, "w+");

	// cargamos los 4 bytes del largo del mensaje (de izquierda a derecha de a bytes)
	for(int i = 3; i >=0; i--) {
		uint8_t byte = inputLength >> (i * 8);
		writeByte(tmp, byte);
	}

	// cargamos el mensaje byte a byte (ahora cargamos todo de una)
	uint8_t bytes[inputLength];
	fread(bytes, 1, inputLength, inputStream);
	saveStream(tmp, bytes, inputLength);

	// cargamos la extension
	size_t extensionLength = strnlen(extension, MAX_FILENAME_LENGTH);
	uint8_t *extensionBytes = (uint8_t *)extension;
	fwrite(extensionBytes, 1, extensionLength + 1, tmp);

	return tmp;
}