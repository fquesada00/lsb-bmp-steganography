#ifndef __CRYPTO_H__
#define __CRYPTO_H__

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum { AES128, AES192, AES256, DES } BlockCipher_t;
typedef enum { ECB, CFB, OFB, CBC } ModeOfOperation_t;

void encryptFile(FILE *file, char *password, BlockCipher_t blockCipher, ModeOfOperation_t modeOfOperation);

size_t decryptFile(uint8_t *cipherText, size_t cipherTextLength, uint8_t *plainText, char *password, BlockCipher_t blockCipher,
				   ModeOfOperation_t modeOfOperation);

#endif
