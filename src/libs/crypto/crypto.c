#include <crypto.h>
#include <openssl/evp.h>
#include <utils.h>

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
uint8_t **deriveKeyAndIv(uint8_t *password, BlockCipher_t blockCipher, ModeOfOperation_t modeOfOperation) {
	EVP_CIPHER *cipher = getCipherFunction(blockCipher, modeOfOperation);
	int keyLength = EVP_CIPHER_key_length(cipher);
	int ivLength = EVP_CIPHER_iv_length(cipher);

	uint8_t out[keyLength + ivLength];
	char *salt = "";
	if (!PKCS5_PBKDF2_HMAC(password, strlen(password), salt, strlen(salt), 1000, EVP_sha256(), keyLength + ivLength, out)) {
		exitWithError("PKCS5_PBKDF2_HMAC failed while deriving key and IV\n");
	}

	uint8_t **keyAndIv = malloc(sizeof(uint8_t *) * 2);
	keyAndIv[0] = malloc(keyLength);
	keyAndIv[1] = malloc(ivLength);

	memcpy(keyAndIv, out, keyLength + ivLength);

	return keyAndIv;
}

// Fuente: https://wiki.openssl.org/index.php/EVP_Symmetric_Encryption_and_Decryption
int encrypt(BlockCipher_t blockCipher, ModeOfOperation_t modeOfOperation, uint8_t *plaintext, int plaintext_len, uint8_t *key,
			uint8_t *iv, unsigned char *ciphertext) {
	EVP_CIPHER_CTX *ctx;

	int len;

	int ciphertext_len;

	/* Create and initialise the context */
	if (!(ctx = EVP_CIPHER_CTX_new()))
		exitWithError("Error creating cipher context\n");

	/*
	 * Initialise the encryption operation. IMPORTANT - ensure you use a key
	 * and IV size appropriate for your cipher
	 * The IV size for *most* modes is the same as the block size.
	 * For AES this is 128 bits
	 */
	if (1 != EVP_EncryptInit_ex(ctx, getCipherFunction(blockCipher, modeOfOperation), NULL, key, iv))
		exitWithError("Error initialising cipher\n");

	/*
	 * Provide the message to be encrypted, and obtain the encrypted output.
	 * EVP_EncryptUpdate can be called multiple times if necessary
	 */
	if (1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
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

int decrypt(EVP_CIPHER *cipher, uint8_t *ciphertext, int ciphertext_len, uint8_t *key, uint8_t *iv, uint8_t *plaintext) {
	EVP_CIPHER_CTX *ctx;

	int len;

	int plaintext_len;

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
	plaintext_len = len;

	/*
	 * Finalise the decryption. Further plaintext bytes may be written at
	 * this stage.
	 */
	if (1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len))
		exitWithError("Error finalising decryption\n");
	plaintext_len += len;

	/* Clean up */
	EVP_CIPHER_CTX_free(ctx);

	return plaintext_len;
}
