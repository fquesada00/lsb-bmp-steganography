#ifndef __LSB1_H__
#define __LSB1_H__

#include <stdint.h>
#include <steganography.h>

void lsb1Hide(FILE *coverImage, FILE *message, FILE *outputImage, char *messageExtension, uint32_t coverImageSize);
StegMessageFormat_t *lsb1Extract(FILE *image, long imageSize, bool isEncrypted);
void writeLsb1Byte(FILE *src, FILE *dest, uint8_t messageByte);
uint8_t readLsb1Byte(FILE *src);

#endif
