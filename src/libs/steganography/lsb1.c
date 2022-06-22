#include <bmp.h>
#include <constants.h>
#include <lsb1.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <steganography.h>
#include <utils.h>
#include <string.h>

uint8_t loadByte(uint8_t byte) {
	uint8_t dest[BYTE_BITS];
	for (int i = BYTE_BITS - 1, j = 0; i != -1; i--, j++) {
		dest[j] = (byte >> i) & 1;
	}
	for (int i = 0; i < BYTE_BITS; i++) {
		printf("%d", dest[i]);
	}
	printf("\n");
	return dest[BYTE_BITS - 1];
}

int lsb1Hide(uint8_t *coverImage, uint8_t *message) { return 0; }

stegMessageFormat_t *lsb1Extract(FILE *image, long imageSize, bool isEncrypted) {
	uint32_t messageLength = 0;

	// Read the message length
	int i;
	// uint8_t str[33];
	for (i = 0; i < sizeof(messageLength) * BYTE_BITS; i++) {
		uint8_t byte = readBmpByte(image);
		// printf("%d - %d\n", i, byte);
		// str[i] = loadByte(byte) + '0';
		messageLength |= byte & 1;
		if (i != (sizeof(messageLength) * BYTE_BITS - 1)) messageLength <<= 1;
	}

	// str[32] = '\0';
	// printf("=====");
	// printf("%s\n", str);
	// printf("%d\n", messageLength);

	// printf("=====");

	if (imageSize < messageLength * BYTE_BITS) { exitWithError("Extracted invalid length for hidden message."); }

	// Read the message
	uint8_t *message = malloc(messageLength);
	int j;
	for (j = 0; j < messageLength; j++) {
		uint8_t byte = readLsb1Byte(image);
		message[j] = byte;
		// i += BYTE_BITS;
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
	while (i < (imageSize - BYTE_BITS) && (c = readLsb1Byte(image)) != '\0') {
		stegMsgFormat->fileExtension = realloc(stegMsgFormat->fileExtension, sizeof(uint8_t) * (k + 1));
		stegMsgFormat->fileExtension[k - 1] = c;
		i += BYTE_BITS;
		k++;
	}
	stegMsgFormat->fileExtension[k - 1] = '\0';

	printf("%s\n", stegMsgFormat->fileExtension);
	printf("%p\n", stegMsgFormat->fileExtension);

	return stegMsgFormat;
}

uint8_t readLsb1Byte(FILE *src) {
	uint8_t lsb1Byte = 0;
	for (int i = 0; i < BYTE_BITS; i++) {
		uint8_t byte = readBmpByte(src);
		// loadByte(byte);
		lsb1Byte |= byte & 1;
		if (i != (BYTE_BITS - 1)) lsb1Byte <<= 1;
	}
	return lsb1Byte;
}
