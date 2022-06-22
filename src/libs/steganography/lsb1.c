#include <constants.h>
#include <stdbool.h>
#include <stdio.h>
#include <steganography.h>
#include <utils.h>
#include <stdint.h>
#include <lsb1.h>

int lsb1Hide(uint8_t *coverImage, uint8_t *message, char *outFilePath) { return 0; }

stegMessageFormat_t *lsb1Extract(uint8_t *image, long imageSize, char *outFilePath, bool isEncrypted) {
	uint32_t messageLength = 0;

	// Read the message length
	int i;
	for (i = 0; i < sizeof(messageLength) * BYTE_BITS; i++) {
		messageLength |= image[i] & 1;
		messageLength <<= 1;
	}

	if (imageSize < messageLength * BYTE_BITS) { exitWithError("Extracted invalid length for hidden message."); }

	// Read the message
	uint8_t *message = malloc(messageLength);
	int j;
	for (j = 0; j < messageLength; j++) {
		message[j] = readLsb1Byte(&image[i]);
		i += BYTE_BITS;
	}

	stegMessageFormat_t *stegMsgFormat = calloc(1, sizeof(stegMessageFormat_t));
	stegMsgFormat->length = messageLength;
	stegMsgFormat->fileData = message;
	stegMsgFormat->fileExtension = NULL;

	if (isEncrypted) { return stegMsgFormat; }

	// allocate memory for empty string
	stegMsgFormat->fileExtension = malloc(sizeof(uint8_t));
	uint8_t c;
	int k = 1;
	while (i < (imageSize - BYTE_BITS) && (c = readLsb1Byte(&image[i])) != '\0') {
		stegMsgFormat->fileExtension = realloc(stegMsgFormat->fileExtension, k + 1);
		stegMsgFormat->fileExtension[k - 1] = c;
		i += BYTE_BITS;
		k++;
	}

	stegMsgFormat->fileExtension[k - 1] = '\0';

	return stegMsgFormat;
}

uint8_t readLsb1Byte(uint8_t *src) {
	uint8_t byte = 0;
	for (int i = 0; i < BYTE_BITS; i++) {
		byte |= src[i] & 1;
		byte <<= 1;
	}
	return byte;
}