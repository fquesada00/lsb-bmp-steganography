#include <args.h>
#include <bmp.h>
#include <lsb1.h>
#include <stdbool.h>
#include <stdio.h>
#include <steganography.h>
#include <string.h>
#include <utils.h>

int main(int argc, char *argv[]) {
	/*
	args_t args = {0};
	parseArgs(&args, argc, argv);
	printf("%s\n", args.in);
	printf("%s\n", args.bitmapFile);
	printf("%s\n", args.out);
	printf("%s\n", args.steganographyMode == LSB1 ? "LSB1" : "LSB4");
	*/
	char *bmpFilePath = "./data/ladoLSB1.bmp";
	BmpHeader header = {0};
	FILE *bmpStream = getStream(bmpFilePath);	
	loadHeader(bmpStream, &header);
	// printf("%d\n", header.type);
	printf("%d\n", header.size);
	// printf("%d\n", header.reserved1);
	// printf("%d\n", header.reserved2);
	// printf("%d\n", header.offset);
	skipOffset(bmpStream, header.offset);


	StegMessageFormat_t *extractedMessage = lsb1Extract(bmpStream, header.size, false);
	closeStream(bmpStream);

	printf("Steg size = %d\n", extractedMessage->length);
	printf("Steg extension = %s\n", extractedMessage->fileExtension);

	saveExtractedMessageToFile(extractedMessage->fileData, extractedMessage->length, extractedMessage->fileExtension, "out");
	// FILE *message = getStream("./data/ejemplo2022/out.png");
	
	// // load the message length
	// FILE *outputImage = fopen("lsb1.bmp", "w");
	// if (outputImage == NULL) {
	// 	exitWithError("Could not open output file.");
	// }

	// copyBmpHeaderAndOffset(bmpStream, outputImage, &header);
	// lsb1Hide(bmpStream, message, outputImage, ".png", header.size - header.offset);

	// closeStream(bmpStream);
	// closeStream(message);
	// closeStream(outputImage);
	return 0;
}
