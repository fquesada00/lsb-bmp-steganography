#ifndef __LSB_H__
#define __LSB_H__

#include <stdbool.h>
#include <stdint.h>
#include <steganography.h>

size_t lsbExtract(FILE *image, long imageSize, uint8_t *extractedMessage, Steganography_t mode, bool isEncrypted);
void lsbHide(FILE *coverImage, FILE *input, FILE *outputImage, uint32_t coverImageLength, Steganography_t mode);
size_t lsbImprovedExtract(FILE *image, long imageSize, uint8_t *extractedMessage, bool isEncrypted);
uint8_t readLsbByte(FILE *src, size_t lsbCount);
void writeLsbByte(FILE *src, FILE *dest, uint8_t messageByte, size_t lsbCount);

#endif
