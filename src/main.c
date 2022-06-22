#include <args.h>
#include <bmp.h>
#include <lsb1.h>
#include <stdbool.h>
#include <steganography.h>
#include <stdio.h>
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
	char *bmpFilePath = "./data/ejemplo2022/ladoLSB1.bmp";
	bmpHeader header = {0};
	FILE *bmpStream = getStream(bmpFilePath);
	loadHeader(bmpStream, &header);
	// printf("%d\n", header.type);
	printf("%d\n", header.size);
	// printf("%d\n", header.reserved1);
	// printf("%d\n", header.reserved2);
	// printf("%d\n", header.offset);
	skipOffset(bmpStream, header.offset);

	// uint8_t i = 32;
	// while(i--) {
	// 	printf("%d\n",readBmpByte(bmpStream));
	// }
	// exit(1);

	stegMessageFormat_t * extractedMessage = lsb1Extract(bmpStream, header.size, false);
	closeStream(bmpStream);

	printf("Steg size = %d\n", extractedMessage->length);
	printf("%p\n", extractedMessage->fileExtension);
	printf("%c\n", extractedMessage->fileData[0]);
	printf("Steg extension = %s\n",extractedMessage->fileExtension);

	// load stream
	size_t CHUNK_BYTES = 1024 * 10;
	size_t bytes_saved = 0;
	size_t remaining_bytes = extractedMessage->length;
	int ended = false;
	char outputFileName[100] = "outputMessage";
	strncat(outputFileName, (const char*)extractedMessage->fileExtension, 100);
	FILE *outStream = createStream(outputFileName);
	uint8_t pngHeader[8] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
	saveStream(outStream, pngHeader, 8);
	while(!ended) {
		size_t bytes_to_write = remaining_bytes > CHUNK_BYTES ? CHUNK_BYTES : remaining_bytes;
		size_t bytesWritten = saveStream(outStream, extractedMessage->fileData + bytes_saved, bytes_to_write);
		bytes_saved += bytesWritten;
		remaining_bytes -= bytesWritten;
		ended = remaining_bytes == 0;
	}
	closeStream(outStream);
}
