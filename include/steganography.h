#ifndef __STEGANOGRAPHY_H__
#define __STEGANOGRAPHY_H__

#include <stdint.h>

typedef enum { LSB1, LSB4, LSBI } steganography_t;
typedef struct {
	uint32_t length;
	uint8_t *fileData;
	uint8_t *fileExtension;
} stegMessageFormat_t;

#endif