#include <arpa/inet.h>
#include <crypto.h>
#include <openssl/evp.h>
#include <stdio.h>
#include <string.h>
#include <utils.h>

static EVP_CIPHER *getCipherFunction(BlockCipher_t blockCipher, ModeOfOperation_t modeOfOperation);
static void deriveKeyAndIv(char *password, EVP_CIPHER *cipher, uint8_t *key, uint8_t *iv);
static size_t encrypt(EVP_CIPHER *cipher, uint8_t *plainText, size_t plainTextLen, uint8_t *key, uint8_t *iv,
					  unsigned char *cipherText);
static size_t decrypt(EVP_CIPHER *cipher, uint8_t *cipherText, size_t cipherTextLen, uint8_t *key, uint8_t *iv,
					  unsigned char *plainText);

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
static void deriveKeyAndIv(char *password, EVP_CIPHER *cipher, uint8_t *key, uint8_t *iv) {
	if (!EVP_BytesToKey(cipher, EVP_sha256(), NULL, (unsigned char *)password, strlen(password), 1, key, iv)) {
		exitWithError("PKCS5_PBKDF2_HMAC failed while deriving key and IV\n");
	}
}

// Fuente: https://wiki.openssl.org/index.php/EVP_Symmetric_Encryption_and_Decryption
static size_t encrypt(EVP_CIPHER *cipher, uint8_t *plainText, size_t plainTextLen, uint8_t *key, uint8_t *iv,
					  unsigned char *cipherText) {
	EVP_CIPHER_CTX *ctx;

	int len;

	size_t cipherTextLen;

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
	if (1 != EVP_EncryptUpdate(ctx, cipherText, &len, plainText, plainTextLen))
		exitWithError("Error encrypting\n");
	cipherTextLen = len;

	/*
	 * Finalise the encryption. Further cipherText bytes may be written at
	 * this stage.
	 */
	if (1 != EVP_EncryptFinal(ctx, cipherText + len, &len))
		exitWithError("Error finalising encryption\n");
	cipherTextLen += len;

	/* Clean up */
	EVP_CIPHER_CTX_cleanup(ctx);

	return cipherTextLen;
}

static size_t decrypt(EVP_CIPHER *cipher, uint8_t *cipherText, size_t cipherTextLen, uint8_t *key, uint8_t *iv,
					  uint8_t *plainText) {
	EVP_CIPHER_CTX *ctx;

	int len;

	size_t plainTextLen;

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
	 * Provide the message to be decrypted, and obtain the plainText output.
	 * EVP_DecryptUpdate can be called multiple times if necessary.
	 */
	if (1 != EVP_DecryptUpdate(ctx, plainText, &len, cipherText, cipherTextLen))
		exitWithError("Error decrypting\n");
	plainTextLen = len;

	/*
	 * Finalise the decryption. Further plainText bytes may be written at
	 * this stage.
	 */
	printf("%d\n", plainTextLen);
	if (1 != EVP_DecryptFinal(ctx, plainText + len, &len))
		exitWithError("Error finalising decryption\n");
	plainTextLen += len;

	/* Clean up */
	EVP_CIPHER_CTX_cleanup(ctx);

	return plainTextLen;
}

void encryptFile(FILE *file, char *password, BlockCipher_t blockCipher, ModeOfOperation_t modeOfOperation) {
	EVP_CIPHER *cipher = getCipherFunction(blockCipher, modeOfOperation);
	printf("cipher: %s\n", EVP_CIPHER_name(cipher));
	uint32_t blockSize = EVP_CIPHER_block_size(cipher);

	uint32_t keyLength = EVP_CIPHER_key_length(cipher);
	uint32_t ivLength = EVP_CIPHER_iv_length(cipher);

	uint8_t key[keyLength];
	uint8_t iv[ivLength];
	deriveKeyAndIv(password, cipher, key, iv);

	// Write plainText to output buffer
	uint32_t fileLength = getFileLength(file);

	uint8_t plainText[fileLength];
	uint32_t cipherTextLength = sizeof(uint32_t) + fileLength + blockSize;
	uint8_t cipherText[cipherTextLength];

	fread(plainText, sizeof(uint8_t), fileLength, file);
	rewind(file);

	uint32_t cipherTextOffset = encrypt(cipher, plainText, fileLength, key, iv, cipherText);
	uint32_t cipherTextOffsetBigEndian = htonl(cipherTextOffset);

	// Write cipherText length to file
	fwrite(&cipherTextOffsetBigEndian, sizeof(uint32_t), 1, file);
	// Write cipherText to file
	fwrite(cipherText, sizeof(uint8_t), cipherTextOffset, file);

	rewind(file);
	printf("Encrypted file length: %ld\n", getFileLength(file));
}

size_t decryptFile(uint8_t *cipherText, size_t cipherTextLength, uint8_t *plainText, char *password, BlockCipher_t blockCipher,
				   ModeOfOperation_t modeOfOperation) {
	EVP_CIPHER *cipher = getCipherFunction(blockCipher, modeOfOperation);

	size_t keyLength = EVP_CIPHER_key_length(cipher);
	size_t ivLength = EVP_CIPHER_iv_length(cipher);

	uint8_t key[keyLength];
	uint8_t iv[ivLength];
	deriveKeyAndIv(password, cipher, key, iv);

	size_t plainTextLength = decrypt(cipher, cipherText, cipherTextLength, key, iv, plainText);

	return plainTextLength;
}
