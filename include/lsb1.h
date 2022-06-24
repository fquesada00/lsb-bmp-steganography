#ifndef __LSB1_H__
#define __LSB1_H__

#include <stdint.h>
#include <steganography.h>

void lsbHide(FILE *coverImage, FILE *input, FILE *outputImage, uint32_t coverImageSize, size_t lsbCount);
StegMessageFormat_t *lsbExtract(FILE *image, long imageSize, size_t lsbCount, bool isEncrypted);
void writeLsb1Byte(FILE *src, FILE *dest, uint8_t messageByte);
uint8_t readLsbByte(FILE *src, size_t lsbCount);
void writeLsbByte(FILE *src, FILE *dest, uint8_t messageByte, size_t lsbCount);
uint8_t readLsb1Byte(FILE *src);

#endif
