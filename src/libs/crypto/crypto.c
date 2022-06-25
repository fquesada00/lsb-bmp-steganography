#include <crypto.h>
#include <openssl/evp.h>
#include <stdio.h>
#include <string.h>
#include <utils.h>

static EVP_CIPHER *getCipherFunction(BlockCipher_t blockCipher, ModeOfOperation_t modeOfOperation);
static void deriveKeyAndIv(char *password, EVP_CIPHER *cipher, uint8_t *keyAndIv);
static size_t encrypt(EVP_CIPHER *cipher, uint8_t *plaintext, size_t plaintextLen, uint8_t *key, uint8_t *iv,
					  unsigned char *ciphertext);
static size_t decrypt(EVP_CIPHER *cipher, uint8_t *ciphertext, size_t ciphertext_len, uint8_t *key, uint8_t *iv,
					  unsigned char *plaintext);

static EVP_CIPHER *getCipherFunction(BlockCipher_t blockCipher, ModeOfOperation_t modeOfOperation) {
	EVP_CIPHER *cipher = NULL;

	switch (blockCipher) {
		case AES128:
			switch (modeOfOperation) {
				case ECB:
					cipher = EVP_aes_128_ecb();
					break;
				case CBC:
					cipher = EVP_aes_128_cbc();
					break;
				case CFB:
					cipher = EVP_aes_128_cfb8();
					break;
				case OFB:
					cipher = EVP_aes_128_ofb();
					break;
				default:
					exitWithError("Invalid mode of operation\n");
			}
			break;
		case AES192:
			switch (modeOfOperation) {
				case ECB:
					cipher = EVP_aes_192_ecb();
					break;
				case CBC:
					cipher = EVP_aes_192_cbc();
					break;
				case CFB:
					cipher = EVP_aes_192_cfb8();
					break;
				case OFB:
					cipher = EVP_aes_192_ofb();
					break;
				default:
					exitWithError("Invalid mode of operation\n");
			}
			break;
		case AES256:
			switch (modeOfOperation) {
				case ECB:
					cipher = EVP_aes_256_ecb();
					break;
				case CBC:
					cipher = EVP_aes_256_cbc();
					break;
				case CFB:
					cipher = EVP_aes_256_cfb8();
					break;
				case OFB:
					cipher = EVP_aes_256_ofb();
					break;
				default:
					exitWithError("Invalid mode of operation\n");
			}
			break;
		case DES:
			switch (modeOfOperation) {
				case ECB:
					cipher = EVP_des_ecb();
					break;
				case CBC:
					cipher = EVP_des_cbc();
					break;
				case CFB:
					cipher = EVP_des_cfb8();
					break;
				case OFB:
					cipher = EVP_des_ofb();
					break;
				default:
					exitWithError("Invalid mode of operation\n");
			}
			break;
		default:
			exitWithError("Invalid block cipher\n");
	}

	return cipher;
}

// Returns dynamically allocated key and IV.
static void deriveKeyAndIv(char *password, EVP_CIPHER *cipher, uint8_t *keyAndIv) {
	int keyLength = EVP_CIPHER_key_length(cipher);
	int ivLength = EVP_CIPHER_iv_length(cipher);

	const unsigned char *salt = (unsigned char *)"";

	if (!PKCS5_PBKDF2_HMAC(password, strlen(password), salt, strlen((char *)salt), 1000, EVP_sha256(), keyLength + ivLength,
						   keyAndIv)) {
		exitWithError("PKCS5_PBKDF2_HMAC failed while deriving key and IV\n");
	}
}

// Fuente: https://wiki.openssl.org/index.php/EVP_Symmetric_Encryption_and_Decryption
static size_t encrypt(EVP_CIPHER *cipher, uint8_t *plaintext, size_t plaintextLen, uint8_t *key, uint8_t *iv,
					  unsigned char *ciphertext) {
	EVP_CIPHER_CTX *ctx;

	int len;

	size_t ciphertext_len;

	/* Create and initialise the context */
	if (!(ctx = EVP_CIPHER_CTX_new()))
		exitWithError("Error creating cipher context\n");

	/*
	 * Initialise the encryption operation. IMPORTANT - ensure you use a key
	 * and IV size appropriate for your cipher
	 * The IV size for *most* modes is the same as the block size.
	 * For AES this is 128 bits
	 */
	if (1 != EVP_EncryptInit_ex(ctx, cipher, NULL, key, iv))
		exitWithError("Error initialising cipher\n");

	/*
	 * Provide the message to be encrypted, and obtain the encrypted output.
	 * EVP_EncryptUpdate can be called multiple times if necessary
	 */
	if (1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintextLen))
		exitWithError("Error encrypting\n");
	ciphertext_len = len;

	/*
	 * Finalise the encryption. Further ciphertext bytes may be written at
	 * this stage.
	 */
	if (1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len))
		exitWithError("Error finalising encryption\n");
	ciphertext_len += len;

	/* Clean up */
	EVP_CIPHER_CTX_free(ctx);

	return ciphertext_len;
}

static size_t decrypt(EVP_CIPHER *cipher, uint8_t *ciphertext, size_t ciphertext_len, uint8_t *key, uint8_t *iv,
					  uint8_t *plaintext) {
	EVP_CIPHER_CTX *ctx;

	int len;

	size_t plaintextLen;

	/* Create and initialise the context */
	if (!(ctx = EVP_CIPHER_CTX_new()))
		exitWithError("Error creating cipher context\n");

	/*
	 * Initialise the encryption operation. IMPORTANT - ensure you use a key
	 * and IV size appropriate for your cipher
	 * The IV size for *most* modes is the same as the block size.
	 * For AES this is 128 bits
	 */
	if (1 != EVP_DecryptInit_ex(ctx, cipher, NULL, key, iv))
		exitWithError("Error initialising cipher\n");

	/*
	 * Provide the message to be decrypted, and obtain the plaintext output.
	 * EVP_DecryptUpdate can be called multiple times if necessary.
	 */
	if (1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
		exitWithError("Error decrypting\n");
	plaintextLen = len;

	/*
	 * Finalise the decryption. Further plaintext bytes may be written at
	 * this stage.
	 */
	if (1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len))
		exitWithError("Error finalising decryption\n");
	plaintextLen += len;

	/* Clean up */
	EVP_CIPHER_CTX_free(ctx);

	return plaintextLen;
}

void encryptFile(FILE *file, char *password, BlockCipher_t blockCipher, ModeOfOperation_t modeOfOperation) {
	EVP_CIPHER *cipher = getCipherFunction(blockCipher, modeOfOperation);
	uint32_t blockSize = EVP_CIPHER_block_size(cipher);

	uint32_t keyLength = EVP_CIPHER_key_length(cipher);
	uint32_t ivLength = EVP_CIPHER_iv_length(cipher);

	uint8_t keyAndIv[keyLength + ivLength];
	deriveKeyAndIv(password, cipher, keyAndIv);
	uint8_t *key = keyAndIv;
	uint8_t *iv = keyAndIv + keyLength;

	uint32_t fileLength = getFileLength(file);
	uint8_t plainText[fileLength];
	uint32_t cipherTextLength = sizeof(uint32_t) + fileLength + blockSize;
	uint8_t cipherText[cipherTextLength];

	fread(plainText, sizeof(uint8_t), fileLength, file);
	rewind(file);

	uint32_t cipherTextOffset = encrypt(cipher, plainText, fileLength, key, iv, cipherText);
	uint32_t cipherTextOffsetBigEndian = htonl(cipherTextOffset);

	// Write ciphertext length to file
	fwrite(&cipherTextOffsetBigEndian, sizeof(uint32_t), 1, file);
	// Write ciphertext to file
	fwrite(cipherText, sizeof(uint8_t), cipherTextOffset, file);
}

size_t decryptFile(uint8_t *cipherText, size_t cipherTextLength, uint8_t *plainText, char *password, BlockCipher_t blockCipher,
				   ModeOfOperation_t modeOfOperation) {
	EVP_CIPHER *cipher = getCipherFunction(blockCipher, modeOfOperation);

	size_t keyLength = EVP_CIPHER_key_length(cipher);
	size_t ivLength = EVP_CIPHER_iv_length(cipher);

	uint8_t keyAndIv[keyLength + ivLength];
	deriveKeyAndIv(password, cipher, keyAndIv);
	uint8_t *key = keyAndIv;
	uint8_t *iv = keyAndIv + keyLength;
	uint8_t dest[keyLength + 1];
	strncpy(dest, key, keyLength);
	dest[keyLength] = '\0';

	printf("Key %s -- IV %s", dest, iv);
	// Write plaintext to output buffer
	size_t plainTextLength = decrypt(cipher, cipherText, cipherTextLength, key, iv, plainText);

	return plainTextLength;
}
