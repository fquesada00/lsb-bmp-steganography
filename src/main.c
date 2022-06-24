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
	FILE *outputImage = NULL;

	if (args.embed) {
		outputImage = createStream(args.out, "w");
		FILE *inputMessage = createStream(args.in, "r");

		copyBmpHeaderAndOffset(coverImage, outputImage, &header);

		FILE *encodedInputTmp = copyEncodedInputToFile(inputMessage, ".png");

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
			StegMessageFormat_t *extractedMessage =
				lsbExtract(coverImage, header.size, getLsbCount(args.steganographyMode), false);

			outputImage = saveExtractedMessageToFile(extractedMessage->fileData, extractedMessage->length,
													 extractedMessage->fileExtension, args.out);
		}
	}

	closeStream(coverImage);
	closeStream(outputImage);

	return 0;
}
