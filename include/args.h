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
	IN,
	BITMAP,
	OUT,
	STEGANOGRAPHY,
} args;

static struct option longOptions[] = {{"embed", no_argument, NULL, EMBED},
									  {"in", required_argument, NULL, IN},
									  {"p", required_argument, NULL, BITMAP},
									  {"out", required_argument, NULL, OUT},
									  {"steg", required_argument, NULL, STEGANOGRAPHY},
									  {0, 0, 0, 0}};

typedef struct args_t {
	bool embed;
	char in[MAX_FILENAME_SIZE];
	char bitmapFile[MAX_FILENAME_SIZE];
	char out[MAX_FILENAME_SIZE];
	Steganography_t steganographyMode;
	// TODO: Add encryption mode
} args_t;

void parseArgs(args_t *args, int argc, char *argv[]);

#endif