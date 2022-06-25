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

		// TODO: sacar el hardcodeo de la extension
		FILE *encodedInputTmp = copyEncodedInputToFile(inputMessage, ".png");

		if (strlen(args.password) > 0) {
			encryptFile(encodedInputTmp, args.password, args.blockCipher, args.modeOfOperation);
			// encodedInputTm = encrypted_size || encrypted_data
		}

		// FILE *ourEncryption = createStream("./ourEncryption", "w");
		// uint32_t encryptedSize = getFileLength(encodedInputTmp);
		// uint8_t encryptedData[encryptedSize];
		// fread(encryptedData, sizeof(uint8_t), encryptedSize, encodedInputTmp);
		// fwrite(encryptedData, sizeof(uint8_t), encryptedSize, ourEncryption);
		// // FILE *encodedInputTmp = createStream("extractedEncryption", "r");
		// rewind(encodedInputTmp);
		// fclose(ourEncryption);

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

		uint8_t extractedMessage[header.size / outputByteSize];
		bool isEncrypted = strlen(args.password) > 0;
		size_t extractedLength = lsbExtract(coverImage, header.size /*no deberia ser header.size - header.offset ?????*/,
											extractedMessage, args.steganographyMode, isEncrypted);
		// FILE *saveEncryptedText = fopen("extractedEncryption", "w");
		// fwrite(extractedMessage, sizeof(uint8_t), extractedLength, saveEncryptedText);
		// fclose(saveEncryptedText);

		uint8_t plainText[extractedLength];
		if (isEncrypted) {
			extractedLength = decryptFile(extractedMessage + sizeof(uint32_t), extractedLength - sizeof(uint32_t), plainText,
										  args.password, args.blockCipher, args.modeOfOperation);
			memcpy(extractedMessage, plainText, extractedLength);
		}

		outputImage = saveExtractedMessageToFile(extractedMessage, extractedLength, args.out);
	}

	closeStream(coverImage);
	closeStream(outputImage);

	return 0;
}
