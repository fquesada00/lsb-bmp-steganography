#ifndef __LSB1_H__
#define __LSB1_H__

#include <stdint.h>
#include <steganography.h>

int lsb1Hide(uint8_t *coverImage, uint8_t *message, char *outFilePath);
stegMessageFormat_t *lsb1Extract(uint8_t *image, long imageSize, char *outFilePath, bool isEncrypted);
uint8_t readLsb1Byte(uint8_t *src);

#endif