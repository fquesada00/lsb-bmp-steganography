#include <args.h>
#include <constants.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <steganography.h>
#include <string.h>
#include <utils.h>

extern char *optarg;

static struct option longOptions[] = {
	{"embed", no_argument, NULL, EMBED},		  {"extract", no_argument, NULL, EXTRACT},
	{"in", required_argument, NULL, IN},		  {"p", required_argument, NULL, BITMAP},
	{"out", required_argument, NULL, OUT},		  {"steg", required_argument, NULL, STEGANOGRAPHY},
	{"a", required_argument, NULL, BLOCK_CIPHER}, {"m", required_argument, NULL, MODE_OF_OPERATION},
	{"pass", required_argument, NULL, PASSWORD},  {0, 0, 0, 0}};

void parseArgs(Args_t *args, int argc, char *argv[]) {
	int index = 0;

	bool missingEmbed = true, missingExtract = true, missingBitmap = true, missingOut = true, missingSteg = true,
		 missingPass = true, missingBlock = true, missingMode = true;

	int option = getopt_long_only(argc, argv, "", longOptions, &index);

	while (option != -1) {
		switch (option) {
			case EMBED:
				args->embed = true;
				missingEmbed = false;
				break;
			case EXTRACT:
				args->extract = true;
				missingExtract = false;
				break;
			case IN:
				strncpy(args->in, optarg, MAX_FILENAME_SIZE);
				break;
			case BITMAP:
				strncpy(args->bitmapFile, optarg, MAX_FILENAME_SIZE);
				missingBitmap = false;
				break;
			case OUT:
				strncpy(args->out, optarg, MAX_FILENAME_SIZE);
				missingOut = false;
				break;
			case STEGANOGRAPHY:
				missingSteg = false;
				if (strncmp(optarg, "LSB1", 4) == 0) {
					args->steganographyMode = LSB1;
				} else if (strncmp(optarg, "LSB4", 4) == 0) {
					args->steganographyMode = LSB4;
				} else if (strncmp(optarg, "LSBI", 4) == 0) {
					args->steganographyMode = LSBI;
				} else {
					exitWithError("Invalid steganography mode\n");
				}
				break;
			case BLOCK_CIPHER:
				if (strncmp(optarg, "aes128", 6) == 0) {
					args->blockCipher = AES128;
				} else if (strncmp("aes192", optarg, 6) == 0) {
					args->blockCipher = AES192;
				} else if (strncmp("aes256", optarg, 6) == 0) {
					args->blockCipher = AES256;
				} else if (strncmp("des", optarg, 3) == 0) {
					args->blockCipher = DES;
				} else {
					exitWithError("Invalid block cipher algorithm\n");
				}
				missingBlock = false;

				break;
			case MODE_OF_OPERATION:
				if (strncmp(optarg, "ecb", 3) == 0) {
					args->modeOfOperation = ECB;
				} else if (strncmp(optarg, "cbc", 3) == 0) {
					args->modeOfOperation = CBC;
				} else if (strncmp(optarg, "cfb", 3) == 0) {
					args->modeOfOperation = CFB;
				} else if (strncmp(optarg, "ofb", 3) == 0) {
					args->modeOfOperation = OFB;
				} else {
					exitWithError("Invalid mode of operation for encryption\n");
				}
				missingMode = false;

				break;
			case PASSWORD:
				strncpy(args->password, optarg, MAX_PASSWORD_LENGTH);
				missingPass = false;
				break;
			default:
				break;
		}

		option = getopt_long_only(argc, argv, "", longOptions, &index);
	}

	bool missingArgs = false;

	if (missingPass) {
		args->password[0] = '\0';
	}

	// catch default arguments for encryption
	if (missingPass && !(missingMode && missingBlock)) {
		fprintf(stderr, "Missing password for encryption\n");
		missingArgs = true;
	} else if (!missingPass && missingMode && missingBlock) {
		fprintf(stderr,
				"Missing mode of operation and block cipher algorithm for encryption. Defaulting to AES128 with CBC mode\n");
		args->blockCipher = AES128;
		args->modeOfOperation = CBC;
	} else if (!missingPass && missingMode && !missingBlock) {
		fprintf(stderr, "Missing mode of operation for encryption. Defaulting to CBC mode\n");
		args->modeOfOperation = CBC;
	} else if (!missingPass && !missingMode && missingBlock) {
		fprintf(stderr, "Missing block cipher algorithm for encryption. Defaulting to AES128\n");
		args->blockCipher = AES128;
	}

	if (missingEmbed && missingExtract) {
		fprintf(stderr, "Missing embed or extract option\n");
		missingArgs = true;
	}

	if (missingBitmap) {
		fprintf(stderr, "Missing bitmap argument\n");
		missingArgs = true;
	}
	if (missingOut) {
		fprintf(stderr, "Missing out argument\n");
		missingArgs = true;
	}
	if (missingSteg) {
		fprintf(stderr, "Missing steg argument\n");
		missingArgs = true;
	}
	if (missingArgs) {
		exit(EXIT_FAILURE);
	}
}

size_t getLsbCount(Steganography_t lsbMode) {
	switch (lsbMode) {
		case LSB1:
			return 1;
		case LSB4:
			return 4;
		default:
			exitWithError("Invalid steganography mode");
			return 0;
	}
}
