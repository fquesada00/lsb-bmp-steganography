#include <args.h>
#include <arpa/inet.h>
#include <bmp.h>
#include <constants.h>
#include <lsb1.h>
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

		FILE *encodedInputTmp = copyEncodedInputToFile(inputMessage, ".png");

		if (strlen(args.password) > 0) {
			encryptFile(encodedInputTmp, args.password, args.blockCipher, args.modeOfOperation);

			FILE *cryptoFile = fopen("./cryptoEmbed.txt", "w");
			uint8_t auxBuff[header.size];
			fread(auxBuff, sizeof(uint8_t), header.size, encodedInputTmp);
			fwrite(auxBuff, sizeof(uint8_t), header.size, cryptoFile);
			fclose(cryptoFile);
		}

		if (args.steganographyMode == LSB1 || args.steganographyMode == LSB4) {
			lsbHide(coverImage, encodedInputTmp, outputImage, header.size - header.offset, getLsbCount(args.steganographyMode));
		}

		closeStream(inputMessage);
		closeStream(encodedInputTmp);

		// remove the encoded input file
		remove(TMP_FILENAME);
	} else if (args.extract) {
		loadHeader(coverImage, &header);
		skipOffset(coverImage, header.offset);

		if (args.steganographyMode == LSB1 || args.steganographyMode == LSB4) {
			size_t lsbCount = getLsbCount(args.steganographyMode);
			uint32_t outputByteSize = BYTE_BITS / lsbCount;

			uint8_t extractedMessage[header.size / outputByteSize];
			bool isEncrypted = strlen(args.password) > 0;
			size_t extractedLength = lsbExtract(coverImage, header.size, extractedMessage, lsbCount, isEncrypted);

			uint8_t plainText[extractedLength];
			if (isEncrypted) {
				FILE *cryptoFile = fopen("./cryptoExtracted.txt", "w");
				fwrite(extractedMessage, sizeof(uint8_t), extractedLength, cryptoFile);
				fclose(cryptoFile);
				extractedLength = decryptFile(extractedMessage, extractedLength, plainText, args.password, args.blockCipher,
											  args.modeOfOperation);
				memcpy(extractedMessage, plainText, extractedLength);
			}

			outputImage = saveExtractedMessageToFile(extractedMessage, extractedLength, args.out);
		}
	}

	closeStream(coverImage);
	closeStream(outputImage);

	return 0;
}
