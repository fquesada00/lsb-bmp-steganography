#include <args.h>
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
	FILE *outputImage = createStream(args.out, "w");

	if (args.embed) {
		FILE *inputMessage = createStream(args.in, "r");

		copyBmpHeaderAndOffset(coverImage, outputImage, &header);

		FILE *encodedInput = copyEncodedInputToFile(inputMessage, ".png");

		lsb1Hide(coverImage, encodedInput, outputImage, header.size - header.offset);

		closeStream(inputMessage);
		closeStream(encodedInput);

		// remove the encoded input file
		remove(TMP_FILENAME);
	} else {
		loadHeader(coverImage, &header);
		skipOffset(coverImage, header.offset);

		StegMessageFormat_t *extractedMessage = lsb1Extract(coverImage, header.size, false);

		saveExtractedMessageToFile(extractedMessage->fileData, extractedMessage->length, extractedMessage->fileExtension,
								   args.out);
	}

	closeStream(coverImage);
	closeStream(outputImage);

	return 0;
}
