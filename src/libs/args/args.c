#include <args.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <steganography.h>
#include <string.h>
#include <utils.h>

extern char *optarg;

void parseArgs(Args_t *args, int argc, char *argv[]) {
	int index = 0;

	bool missingBitmap = true, missingOut = true, missingSteg = true;

	int option = getopt_long_only(argc, argv, "", longOptions, &index);

	while (option != -1) {
		switch (option) {
			case EMBED:
				args->embed = true;
				break;
			case EXTRACT:
				args->extract = true;
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
					fprintf(stderr, "Invalid steganography mode\n");
					exit(EXIT_FAILURE);
				}
				break;
			default:
				break;
		}

		option = getopt_long_only(argc, argv, "", longOptions, &index);
	}

	bool missingArgs = false;

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
