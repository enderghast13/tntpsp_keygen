/* SPDX-License-Identifier: 0BSD */
#ifndef PSP2CIPHER_H__
#define PSP2CIPHER_H__

/* PSP2Cipher crypto modes */
enum PSP2CIPHER_MODE {
	PSP2CIPHER_DECRYPT = 0,
	PSP2CIPHER_ENCRYPT = 1,
};

/* PSP2Cipher status/error codes */
enum PSP2CIPHER_ERROR {
	PSP2CIPHER_SUCCESS      = 0,
	PSP2CIPHER_ERR_BAD_MODE = -1,
};

/* PSP2Cipher encryption/decryption context */
typedef struct {
	unsigned char key[4];
} psp2cipher_context;

/**
 * Set the encryption/decryption key for the PSP2Cipher context
 *
 * @param ctx
 *     context to set the key for
 * @param key
 *     the 32-bit encryption/decryption key to use
 */
void psp2cipher_setkey(psp2cipher_context *ctx, const unsigned char key[4]);

/**
 * Encrypt/decrypt a 16-byte block using PSP2Cipher in ECB mode
 *
 * @param ctx
 *     context for the encryption/decryption, set up using `psp2cipher_setkey()`
 * @param mode
 *     set to either `PSP2CIPHER_ENCRYPT` to encrypt the block or
 *     `PSP2CIPHER_DECRYPT` to decrypt the block
 * @param input
 *     input buffer to encrypt/decrypt
 * @param output
 *     output buffer to write encrypted/decrypted data into
 *
 * @returns
 *     `PSP2CIPHER_SUCCESS` on success, `PSP2CIPHER_ERR_BAD_MODE` if the `mode`
 *     parameter was invalid
 */
int psp2cipher_crypt_ecb(const psp2cipher_context *ctx, int mode,
                         const unsigned char input[16],
                         unsigned char output[16]);

#endif  // PSP2CIPHER_H__
