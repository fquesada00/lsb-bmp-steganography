#include <args.h>
#include <arpa/inet.h>
#include <bmp.h>
#include <constants.h>
#include <lsb.h>
#include <stdbool.h>
#include <stdio.h>
#include <steganography.h>
#include <string.h>
#include <utils.h>

int main(int argc, char *argv[]) {
	Args_t args = {0};
	parseArgs(&args, argc, argv);

	BmpHeader header = {0};

	FILE *coverImage = createStream(args.bitmapFile, "r");
	FILE *outputImage = NULL;

	if (args.embed) {
		outputImage = createStream(args.out, "w");
		FILE *inputMessage = createStream(args.in, "r");

		copyBmpHeaderAndOffset(coverImage, outputImage, &header);

		char *extension = strchr(args.in, '.');

		FILE *encodedInputTmp = copyEncodedInputToFile(inputMessage, extension);

		bool encrypt = strlen(args.password) > 0;
		if (encrypt) {
			encryptFile(encodedInputTmp, args.password, args.blockCipher, args.modeOfOperation);
		}

		lsbHide(coverImage, encodedInputTmp, outputImage, header.size - header.offset, args.steganographyMode);

		closeStream(inputMessage);
		closeStream(encodedInputTmp);

		// remove the encoded input file
		remove(TMP_FILENAME);
	} else if (args.extract) {

		loadHeader(coverImage, &header);

		skipOffset(coverImage, header.offset);

		size_t lsbCount = getLsbCount(args.steganographyMode);
		uint32_t outputByteSize = BYTE_BITS / lsbCount;

		uint8_t *extractedMessage = calloc(header.size / outputByteSize, sizeof(uint8_t));

		bool isEncrypted = strnlen(args.password, MAX_PASSWORD_LENGTH) > 0;

		size_t extractedLength =
			lsbExtract(coverImage, header.size - header.offset, extractedMessage, args.steganographyMode, isEncrypted);

		uint8_t *plainText = calloc(extractedLength, sizeof(uint8_t));
		if (isEncrypted) {
			extractedLength = decryptFile(extractedMessage + sizeof(uint32_t), extractedLength - sizeof(uint32_t), plainText,
										  args.password, args.blockCipher, args.modeOfOperation);
			memcpy(extractedMessage, plainText, extractedLength);
		}

		outputImage = saveExtractedMessageToFile(extractedMessage, extractedLength, args.out);

		free(extractedMessage);
		free(plainText);
	}

	closeStream(coverImage);
	closeStream(outputImage);

	return 0;
}
