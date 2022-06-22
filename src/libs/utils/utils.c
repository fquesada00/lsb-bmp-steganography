#include <stdio.h>
#include <stdlib.h>

void exitWithError(char *message) {
	perror(message);
	exit(EXIT_FAILURE);
}