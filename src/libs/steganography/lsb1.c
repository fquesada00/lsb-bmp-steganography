#include <bmp.h>
#include <constants.h>
#include <lsb1.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <steganography.h>
#include <string.h>
#include <utils.h>

void lsbHide(FILE *coverImage, FILE *input, FILE *outputImage, uint32_t coverImageLength, size_t lsbCount) {

	uint32_t messageLength = getFileLength(input);
	uint32_t outputByteSize = BYTE_BITS / lsbCount;

	if (coverImageLength < messageLength * outputByteSize) {
		exitWithError("Cover image is too small to hide the message");
	}

	// cargamos el mensaje byte a byte
	for (int i = 0; i < messageLength; i++) {
		uint8_t messageByte = readByte(input);
		writeLsbByte(coverImage, outputImage, messageByte, lsbCount);
	}

	// cargamos los bytes restantes del archivo
	uint32_t remainingLength = coverImageLength - messageLength * outputByteSize;
	uint8_t dump[remainingLength];
	// TODO: Check	errors
	fread((void *)dump, 1, remainingLength, coverImage);
	fwrite((void *)dump, 1, remainingLength, outputImage);
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

void writeLsbByte(FILE *src, FILE *dest, uint8_t messageByte, size_t lsbCount) {
	if (lsbCount > BYTE_BITS) {
		exitWithError("LSB count cannot be bigger than a byte");
	}

	if (BYTE_BITS % lsbCount != 0) {
		exitWithError("LSB count must be a divisor of 8");
	}

	uint8_t mask = BIT_MASK(uint8_t, lsbCount);
	int bytes = BYTE_BITS / lsbCount;

	for (int i = bytes - 1; i >= 0; i--) {
		uint8_t imageByte = readByte(src);
		uint8_t lsbs = (messageByte >> (lsbCount * i)) & mask;
		imageByte &= ~mask;
		imageByte |= lsbs;
		writeByte(dest, imageByte);
	}
}

StegMessageFormat_t *lsbExtract(FILE *image, long imageSize, size_t lsbCount, bool isEncrypted) {
	uint32_t outputByteSize = BYTE_BITS / lsbCount;

	uint32_t messageLength = 0;

	//  Read the message length
	for (int i = 0; i < sizeof(messageLength); i++) {
		messageLength |= readLsbByte(image, lsbCount);
		if (i != sizeof(messageLength) - 1)
			messageLength <<= BYTE_BITS;
	}

	if (imageSize < messageLength * outputByteSize) {
		exitWithError("Extracted invalid length for hidden message.");
	}

	// Read the message
	uint8_t *message = malloc(messageLength);
	for (int j = 0; j < messageLength; j++) {
		uint8_t messageByte = readLsbByte(image, lsbCount);
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
	while ((c = readLsbByte(image, lsbCount)) != '\0') {
		stegMsgFormat->fileExtension = realloc(stegMsgFormat->fileExtension, sizeof(uint8_t) * (k + 1));
		stegMsgFormat->fileExtension[k - 1] = c;
		k++;
	}
	stegMsgFormat->fileExtension[k - 1] = '\0';

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

uint8_t readLsbByte(FILE *src, size_t lsbCount) {
	if (lsbCount > BYTE_BITS) {
		exitWithError("LSB count cannot be bigger than a byte");
	}

	if (BYTE_BITS % lsbCount != 0) {
		exitWithError("LSB count must be a divisor of 8");
	}

	uint8_t mask = BIT_MASK(uint8_t, lsbCount);
	int bytes = BYTE_BITS / lsbCount;

	uint8_t ret = 0;
	for (int i = 0; i < bytes; i++) {
		uint8_t messageByte = readByte(src);
		ret |= messageByte & mask;
		if (i != (bytes - 1))
			ret <<= lsbCount;
	}
	return ret;
}
