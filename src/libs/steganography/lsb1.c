#include <args.h>
#include <arpa/inet.h>
#include <bmp.h>
#include <constants.h>
#include <lsb1.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <steganography.h>
#include <string.h>
#include <utils.h>

static size_t lsbNExtract(FILE *image, long imageSize, uint8_t *extractedMessage, size_t n, bool isEncrypted);
static void lsbNHide(FILE *coverImage, FILE *input, FILE *outputImage, uint32_t coverImageSize, size_t lsbCount);
void lsbImprovedHide(FILE *coverImage, FILE *input, FILE *outputImage, uint32_t coverImageLength);

void lsbNHide(FILE *coverImage, FILE *input, FILE *outputImage, uint32_t coverImageLength, size_t n) {

	uint32_t messageLength = getFileLength(input);
	uint32_t outputByteSize = BYTE_BITS / n;

	if (coverImageLength < messageLength * outputByteSize) {
		exitWithError("Cover image is too small to hide the message");
	}

	// cargamos el mensaje byte a byte
	for (int i = 0; i < messageLength; i++) {
		uint8_t messageByte = readByte(input);
		writeLsbByte(coverImage, outputImage, messageByte, n);
	}

	// cargamos los bytes restantes del archivo
	uint32_t remainingLength = coverImageLength - messageLength * outputByteSize;
	uint8_t *dump = calloc(remainingLength, sizeof(uint8_t));

	fread((void *)dump, 1, remainingLength, coverImage);

	if (ferror(coverImage) != 0) {
		exitWithError("Error reading from cover image");
	}

	fwrite((void *)dump, 1, remainingLength, outputImage);

	if (ferror(coverImage) != 0) {
		exitWithError("Error writing from cover image");
	}

	free(dump);
}

void writeLsbByte(FILE *src, FILE *dest, uint8_t messageByte, size_t n) {
	if (n > BYTE_BITS) {
		exitWithError("LSB count cannot be bigger than a byte");
	}

	if (BYTE_BITS % n != 0) {
		exitWithError("LSB count must be a divisor of 8");
	}

	uint8_t mask = BIT_MASK(uint8_t, n);
	int bytes = BYTE_BITS / n;

	for (int i = bytes - 1; i >= 0; i--) {
		uint8_t imageByte = readByte(src);
		uint8_t lsbs = readLsbs((messageByte >> (n * i)), n);
		imageByte &= ~mask;
		imageByte |= lsbs;
		writeByte(dest, imageByte);
	}
}

void lsbHide(FILE *coverImage, FILE *input, FILE *outputImage, uint32_t coverImageLength, Steganography_t mode) {
	switch (mode) {
		case LSB1:
			lsbNHide(coverImage, input, outputImage, coverImageLength, getLsbCount(mode));
			break;
		case LSB4:
			lsbNHide(coverImage, input, outputImage, coverImageLength, getLsbCount(mode));
			break;
		case LSBI:
			lsbImprovedHide(coverImage, input, outputImage, coverImageLength);
			break;
		default:
			exitWithError("Invalid steganography mode");
	}
}

size_t lsbExtract(FILE *image, long imageSize, uint8_t *extractedMessage, Steganography_t mode, bool isEncrypted) {
	switch (mode) {
		case LSB1:
			return lsbNExtract(image, imageSize, extractedMessage, getLsbCount(mode), isEncrypted);
		case LSB4:
			return lsbNExtract(image, imageSize, extractedMessage, getLsbCount(mode), isEncrypted);
		case LSBI:
			return lsbImprovedExtract(image, imageSize, extractedMessage, isEncrypted);
		default:
			exitWithError("Invalid steganography mode");
	}
	return 0;
}

static size_t lsbNExtract(FILE *image, long imageSize, uint8_t *extractedMessage, size_t n, bool isEncrypted) {
	uint32_t outputByteSize = BYTE_BITS / n;

	uint32_t messageLength = 0;

	//  Read the message length
	for (int i = 0; i < sizeof(messageLength); i++) {
		messageLength |= readLsbByte(image, n);
		if (i != sizeof(messageLength) - 1)
			messageLength <<= BYTE_BITS;
	}

	// Copy the message length to the output buffer
	uint32_t messageLengthBigEndian = htonl(messageLength);
	memcpy(extractedMessage, &messageLengthBigEndian, sizeof(messageLength));

	if (imageSize < messageLength * outputByteSize) {
		exitWithError("Invalid Extracted length for hidden message.");
	}

	// Read the message and copy it to the output buffer
	for (int j = 0; j < messageLength; j++) {
		uint8_t messageByte = readLsbByte(image, n);
		extractedMessage[j + sizeof(messageLength)] = messageByte;
	}

	// Encrypted messages do not have an extension
	if (isEncrypted) {
		return messageLength + sizeof(messageLength);
	}

	// Read the extension, which is null terminated
	uint8_t c;
	int k = 0;
	while ((c = readLsbByte(image, n)) != '\0') {
		extractedMessage[sizeof(messageLength) + messageLength + k] = c;
		k++;
	}
	extractedMessage[sizeof(messageLength) + messageLength + k] = '\0';

	return sizeof(messageLength) + messageLength + k + 1;
}

uint8_t readLsbByte(FILE *src, size_t n) {
	if (n > BYTE_BITS) {
		exitWithError("LSB count cannot be bigger than a byte");
	}

	if (BYTE_BITS % n != 0) {
		exitWithError("LSB count must be a divisor of 8");
	}

	uint8_t mask = BIT_MASK(uint8_t, n);
	int bytes = BYTE_BITS / n;

	uint8_t ret = 0;
	for (int i = 0; i < bytes; i++) {
		uint8_t messageByte = readByte(src);
		ret |= messageByte & mask;
		if (i != (bytes - 1))
			ret <<= n;
	}
	return ret;
}

void lsbImprovedHide(FILE *coverImage, FILE *input, FILE *outputImage, uint32_t coverImageLength) {

	size_t preservedPatternsCount[LSBIMPROVED_PATTERNS] = {0};
	size_t invertedPatternsCount[LSBIMPROVED_PATTERNS] = {0}; // [00, 01, 10, 00]

	// recorrer todo el archivo de input y contar la cantidad de cada patron
	uint32_t messageLength = getFileLength(input);
	long coverImageStartPos = ftell(coverImage);

	// start after bytes stored for patterns
	fseek(coverImage, LSBIMPROVED_PATTERNS, SEEK_CUR);
	for (int i = 0; i < messageLength; i++) {
		uint8_t messageByte = readByte(input);
		for (int j = 0; j < BYTE_BITS; j++) {
			uint8_t coverByte = readByte(coverImage);
			uint8_t patternIndex = readLsbs((coverByte >> 1), 2);
			bool inverted = readNthBit(coverByte, BYTE_BITS - 1) ^ readNthBit(messageByte, j);

			if (inverted) {
				invertedPatternsCount[patternIndex]++;
			} else {
				preservedPatternsCount[patternIndex]++;
			}
		}
	}

	bool invertPatterns[LSBIMPROVED_PATTERNS] = {false};

	// determinar si se debe invertir cada patron (si la # de invertidos es mayor a preservados, entonces se invierte)

	rewind(input);
	fseek(coverImage, coverImageStartPos, SEEK_SET);

	for (int i = 0; i < LSBIMPROVED_PATTERNS; i++) {
		if (preservedPatternsCount[i] < invertedPatternsCount[i]) {
			invertPatterns[i] = true;
		}
	}

	// escribir patrones en el archivo de salida, se mueven 4 bytes
	for (int i = 0; i < LSBIMPROVED_PATTERNS; i++) {
		uint8_t imageByte = readByte(coverImage);
		imageByte &= ~(BIT_MASK(uint8_t, 1));
		imageByte |= invertPatterns[i];
		writeByte(outputImage, imageByte);
	}

	FILE *tmpInput = fopen(TMP_LSBI_FILENAME, "w+");
	// invertir los patrones que sean necesarios
	for (int i = 0; i < messageLength; i++) {

		uint8_t messageByte = readByte(input);
		uint8_t newMessageByte = 0;

		for (int j = 0; j < BYTE_BITS; j++) {
			uint8_t coverByte = readByte(coverImage);
			uint8_t patternIndex = readLsbs((coverByte >> 1), 2);

			bool invert = invertPatterns[patternIndex];
			if (invert) {
				newMessageByte |= readNthBit(~messageByte, j);
			} else {
				newMessageByte |= readNthBit(messageByte, j);
			}

			if (j < (BYTE_BITS - 1)) {
				newMessageByte <<= 1;
			}
		}
		writeByte(tmpInput, newMessageByte);
	}
	rewind(tmpInput);
	fseek(coverImage, coverImageStartPos + LSBIMPROVED_PATTERNS, SEEK_SET);

	lsbNHide(coverImage, tmpInput, outputImage, coverImageLength - LSBIMPROVED_PATTERNS, 1);

	fclose(tmpInput);
	remove(TMP_LSBI_FILENAME);
}

size_t lsbImprovedExtract(FILE *image, long imageSize, uint8_t *extractedMessage, bool isEncrypted) {
	// Extract invertPatterns (4 bits)
	bool patterns[LSBIMPROVED_PATTERNS];
	for (int i = 0; i < LSBIMPROVED_PATTERNS; i++) {
		patterns[i] = readNthBit(readByte(image), 7);
		// [0] => 00, [1] => 01, [2] => 10, [3] => 11
	}

	FILE *tmp = fopen(TMP_LSBI_FILENAME, "w+");

	// invertimos los patrones que corresponden
	long startPos = ftell(image);

	for (int i = 0; i < imageSize - startPos; i++) {
		uint8_t coverByte = readByte(image);
		int bytePatternIndex = readLsbs(coverByte >> 1, 2);
		bool invert = patterns[bytePatternIndex];
		writeByte(tmp, coverByte ^ invert);
	}

	fseek(tmp, 0, SEEK_SET);
	fseek(image, startPos, SEEK_SET);

	size_t bytesExtracted = lsbNExtract(tmp, imageSize - sizeof(uint32_t), extractedMessage, 1, isEncrypted);

	fclose(tmp);
	remove(TMP_LSBI_FILENAME);

	return bytesExtracted;
}
