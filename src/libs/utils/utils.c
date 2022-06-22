#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

void exitWithError(char *message) {
	perror(message);
	exit(EXIT_FAILURE);
}

int loadStream(FILE *stream, uint8_t *dest, size_t bytes) {
	size_t bytesRead = fread((void *) dest, 1, bytes, stream);
	// check if EOF is reached
}

size_t saveStream(FILE *stream, uint8_t *src, size_t bytes) {
	size_t bytesWritten = fwrite((void *) src, 1, bytes, stream);
	return bytesWritten;
}