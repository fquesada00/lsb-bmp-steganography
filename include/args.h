#ifndef __ARGS_H__
#define __ARGS_H__
#include <getopt.h>
#include <stdbool.h>
#include <stdlib.h>
#include <steganography.h>
enum MAX_SIZES {
	MAX_FILENAME_SIZE = 64,
};

typedef enum args {
	EMBED,
	EXTRACT,
	IN,
	BITMAP,
	OUT,
	STEGANOGRAPHY,
} args;

static struct option longOptions[] = {{"embed", no_argument, NULL, EMBED},
									  {"extract", no_argument, NULL, EXTRACT},
									  {"in", required_argument, NULL, IN},
									  {"p", required_argument, NULL, BITMAP},
									  {"out", required_argument, NULL, OUT},
									  {"steg", required_argument, NULL, STEGANOGRAPHY},
									  {0, 0, 0, 0}};

typedef struct Args_t {
	bool embed;
	bool extract;
	char in[MAX_FILENAME_SIZE];
	char bitmapFile[MAX_FILENAME_SIZE];
	char out[MAX_FILENAME_SIZE];
	Steganography_t steganographyMode;
	// TODO: Add encryption mode
} Args_t;

void parseArgs(Args_t *args, int argc, char *argv[]);

size_t getLsbCount(Steganography_t lsbMode);

#endif
