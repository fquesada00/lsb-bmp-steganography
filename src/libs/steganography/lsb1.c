#include <bmp.h>
#include <constants.h>
#include <lsb1.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <steganography.h>
#include <string.h>
#include <utils.h>

void lsb1Hide(FILE *coverImage, FILE *message, FILE *outputImage, char *fileExtension, uint32_t coverImageLength) {

	uint32_t messageLength = getFileLength(message);
	size_t extensionLength = strnlen(fileExtension, MAX_FILENAME_LENGTH);

	size_t bytesToHide = (messageLength + sizeof(messageLength) + extensionLength);

	if (coverImageLength < bytesToHide * BYTE_BITS) {
		exitWithError("Cover image is too small to hide the message");
	}

	// cargamos los 4 bytes del largo del mensaje (de izquierda a derecha de a bytes)
	for (int i = sizeof(messageLength) - 1; i >= 0; i--) {
		int byteShift = i * BYTE_BITS;
		writeLsb1Byte(coverImage, outputImage, messageLength >> byteShift);
	}

	// cargamos el mensaje byte a byte
	for (int i = 0; i < messageLength; i++) {
		uint8_t messageByte = readByte(message);
		writeLsb1Byte(coverImage, outputImage, messageByte);
	}

	// cargamos la extension
	uint8_t *extensionBytes = (uint8_t *)fileExtension;
	for (int i = 0; i < (extensionLength + 1); i++) {
		uint8_t messageByte = extensionBytes[i];
		writeLsb1Byte(coverImage, outputImage, messageByte);
	}

	// cargamos los bytes restantes del archivo
	uint32_t remainingLength = coverImageLength - bytesToHide * BYTE_BITS;
	uint8_t *remainingBytes = (uint8_t *)malloc(remainingLength);
	// TODO: Check	errors
	fread(remainingBytes, 1, remainingLength, coverImage);
	fwrite(remainingBytes, 1, remainingLength, outputImage);
}

void writeLsb1Byte(FILE *src, FILE *dest, uint8_t messageByte) {
	for (int i = BYTE_BITS - 1; i >= 0; i--) {
		uint8_t imageByte = readByte(src);
		uint8_t bit = (messageByte >> i) & 0x01;
		imageByte &= ~0x01;
		imageByte |= bit;
		writeByte(dest, imageByte);
	}
}

StegMessageFormat_t *lsb1Extract(FILE *image, long imageSize, bool isEncrypted) {
	uint32_t messageLength = 0;

	// Read the message length
	int i;
	for (i = 0; i < sizeof(messageLength) * BYTE_BITS; i++) {
		uint8_t messageByte = readByte(image);
		messageLength |= messageByte & 0x01;
		if (i != (sizeof(messageLength) * BYTE_BITS - 1))
			messageLength <<= 1;
	}

	if (imageSize < messageLength * BYTE_BITS) {
		exitWithError("Extracted invalid length for hidden message.");
	}

	// Read the message
	uint8_t *message = malloc(messageLength);
	int j;
	for (j = 0; j < messageLength; j++) {
		uint8_t messageByte = readLsb1Byte(image);
		message[j] = messageByte;
	}

	StegMessageFormat_t *stegMsgFormat = calloc(1, sizeof(StegMessageFormat_t));
	stegMsgFormat->length = messageLength;
	stegMsgFormat->fileData = message;
	stegMsgFormat->fileExtension = NULL;

	if (isEncrypted) {
		return stegMsgFormat;
	}

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
		uint8_t messageByte = readByte(src);
		lsb1Byte |= messageByte & 0x01;
		if (i != (BYTE_BITS - 1))
			lsb1Byte <<= 1;
	}
	return lsb1Byte;
}