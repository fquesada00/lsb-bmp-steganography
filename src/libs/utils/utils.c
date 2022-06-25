#include <arpa/inet.h>
#include <constants.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <utils.h>

void exitWithError(char *message) {
	perror(message);
	exit(EXIT_FAILURE);
}

FILE *createStream(char *path, const char *mode) {
	printf("Opening file %s\n", path);
	FILE *stream = fopen(path, mode);
	if (stream == NULL) {
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
		exitWithError("Error close bmp stream");
	}
}

FILE *saveExtractedMessageToFile(uint8_t *extractedMessage, uint32_t length, char *outputFileName) {
	// Parse extracted message
	uint8_t *message = extractedMessage;
	uint32_t messageLength = ntohl(((uint32_t *)message)[0]);

	// Parse file extension
	uint8_t *fileExtension = message + sizeof(messageLength) + messageLength;
	if (strnlen((char *)fileExtension, MAX_FILENAME_SIZE + 1) > MAX_FILENAME_SIZE) {
		exitWithError("Error: File extension in extracted message is too long or may not be null terminated\n");
	}

	// Check file extensions starts with .
	if (((char)fileExtension[0]) != '.') {
		exitWithError("Error: File extension in extracted message does not start with a dot\n");
	}

	// create new string with file name and extension
	size_t fileNameLength = strlen(outputFileName) + strlen((char *)fileExtension) + 1;
	char fileName[fileNameLength];

	strcpy(fileName, outputFileName);
	strcat(fileName, (char *)fileExtension);

	FILE *outStream = createStream(fileName, "w");

	saveStream(outStream, extractedMessage + sizeof(messageLength), messageLength);

	return outStream;
}

uint32_t getFileLength(FILE *stream) {

	long startPos = ftell(stream);
	fseek(stream, 0L, SEEK_END);
	long imageSize = ftell(stream);
	fseek(stream, startPos, SEEK_SET);
	return (uint32_t)imageSize;
}

uint8_t readByte(FILE *stream) {
	uint8_t byte;
	fread(&byte, 1, 1, stream);
	if (ferror(stream) != 0) {
		exitWithError("Error reading byte from file");
	}
	if (feof(stream) != 0) {
		exitWithError("Reached EOF reading byte from file");
	}

	return byte;
}

void writeByte(FILE *stream, uint8_t byte) {
	fwrite(&byte, 1, 1, stream);
	if (ferror(stream) != 0) {
		exitWithError("Error writing byte to file");
	}
}

FILE *copyEncodedInputToFile(FILE *inputStream, char *extension) {
	uint32_t inputLength = getFileLength(inputStream);
	uint32_t inputLengthBigEndian = htonl(inputLength);
	FILE *tmp = createStream(TMP_FILENAME, "w+");

	// cargamos los 4 bytes del largo del mensaje (de izquierda a derecha de a bytes)
	fwrite(&inputLengthBigEndian, sizeof(inputLengthBigEndian), 1, tmp);

	// cargamos el mensaje byte a byte (ahora cargamos todo de una)
	uint8_t bytes[inputLength];
	fread(bytes, 1, inputLength, inputStream);
	saveStream(tmp, bytes, inputLength);

	// cargamos la extension
	size_t extensionLength = strnlen(extension, MAX_FILENAME_SIZE);
	uint8_t *extensionBytes = (uint8_t *)extension;
	fwrite(extensionBytes, 1, extensionLength + 1, tmp);
	rewind(tmp);

	return tmp;
}

uint8_t readLsbs(uint8_t byte, uint8_t n) { return byte & (BIT_MASK(uint8_t, n)); }

uint8_t readNthBit(uint8_t byte, uint8_t n) { return (byte >> (BYTE_BITS - 1 - n)) & (BIT_MASK(uint8_t, 1)); }
