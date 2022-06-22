#include <args.h>
#include <bmp.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
	/*
	args_t args = {0};
	parseArgs(&args, argc, argv);
	printf("%s\n", args.in);
	printf("%s\n", args.bitmapFile);
	printf("%s\n", args.out);
	printf("%s\n", args.steganographyMode == LSB1 ? "LSB1" : "LSB4");
	*/
	char *bmpFilePath = "./sample_640x426.bmp";
	bmpHeader header = {0};
	FILE *bmpStream = getStream(bmpFilePath);
	loadHeader(bmpStream, &header);
	printf("%d\n", header.type);
	printf("%d\n", header.size);
	printf("%d\n", header.reserved1);
	printf("%d\n", header.reserved2);
	printf("%d\n", header.offset);
	skipOffset(bmpStream, header.offset);

	char pixel;
	bool reachedEof;
	int iterations = 10;
	while (iterations) {
		readBmpByte(bmpStream, &pixel);
		printf("pixel: %d\n", pixel);
		iterations--;
	};
	closeStream(bmpStream);
}
