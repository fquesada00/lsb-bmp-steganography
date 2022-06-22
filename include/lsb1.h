#ifndef __LSB1_H__
#define __LSB1_H__

#include <stdint.h>
#include <steganography.h>

int lsb1Hide(uint8_t *coverImage, uint8_t *message);
stegMessageFormat_t *lsb1Extract(FILE *image, long imageSize, bool isEncrypted);
uint8_t readLsb1Byte(FILE *src);

#endif