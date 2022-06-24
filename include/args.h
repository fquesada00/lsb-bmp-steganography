#ifndef __ARGS_H__
#define __ARGS_H__
#include <constants.h>
#include <crypto.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdlib.h>
#include <steganography.h>

typedef enum args { EMBED, EXTRACT, IN, BITMAP, OUT, STEGANOGRAPHY, BLOCK_CIPHER, MODE_OF_OPERATION, PASSWORD } args;

static struct option longOptions[] = {
	{"embed", no_argument, NULL, EMBED},		  {"extract", no_argument, NULL, EXTRACT},
	{"in", required_argument, NULL, IN},		  {"p", required_argument, NULL, BITMAP},
	{"out", required_argument, NULL, OUT},		  {"steg", required_argument, NULL, STEGANOGRAPHY},
	{"a", required_argument, NULL, BLOCK_CIPHER}, {"m", required_argument, NULL, MODE_OF_OPERATION},
	{"pass", required_argument, NULL, PASSWORD},  {0, 0, 0, 0}};

typedef struct Args_t {
	bool embed;
	bool extract;
	char in[MAX_FILENAME_SIZE];
	char bitmapFile[MAX_FILENAME_SIZE];
	char out[MAX_FILENAME_SIZE];
	Steganography_t steganographyMode;
	BlockCipher_t blockCipher;
	ModeOfOperation_t modeOfOperation;
	char password[MAX_PASSWORD_LENGTH];
	// TODO: Add encryption mode
} Args_t;

void parseArgs(Args_t *args, int argc, char *argv[]);

size_t getLsbCount(Steganography_t lsbMode);

#endif
