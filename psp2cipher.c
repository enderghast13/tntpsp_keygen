// SPDX-License-Identifier: 0BSD
#include <string.h>
#if (defined(USE_MBEDTLS) && (USE_MBEDTLS == 1))
  #include <mbedtls/sha1.h>
  #define SHA1_DIGEST(in, len, out) mbedtls_sha1(in, len, out)
#else
  #include "sha1.h"
  #define SHA1_DIGEST(in, len, out) SHA1(out, in, len)
#endif

#include "psp2cipher.h"

// Bitwise XOR `n` values in `src` with the value at the same position in `xor`.
#define memxor(src, xor, n)       \
for (size_t i = 0; i < n; i++) {  \
	(src)[i] ^= (xor)[i];         \
}

// Copy `n` bytes from `src` to `dest` in reverse. Returns `dest`.
static void *memcpy_r(void *restrict dest, const void *restrict src, size_t n) {
	unsigned char *dest_p       = dest;
	const unsigned char *src_p  = src;

	for (size_t i = 0; i < n; i++) {
		dest_p[i] = src_p[n-i-1];
	}

	return dest;
}

void psp2cipher_setkey(psp2cipher_context *ctx, const unsigned char key[4]) {
	memcpy(ctx->key, key, 4);
}

// Encrypt a 16-byte block using the supplied `ctx` from `input` into `output`.
static void psp2cipher_encrypt_ecb(const psp2cipher_context *ctx,
                                   const unsigned char input[16],
                                   unsigned char output[16]) {
	unsigned char block[0x40] = {0};
	unsigned char frame_1[8];
	unsigned char frame_2[8];
	unsigned char frame_3[8];
	unsigned char frame_4[8];
	unsigned char digest[20];

	// round 1
	memcpy(block, ctx->key, 2);
	memcpy_r(&block[0x38], input, 8);
	SHA1_DIGEST(block, 0x40, digest);
	memcpy(frame_1, digest, 8);
	memxor(frame_1, &input[8], 8);

	// round 2
	memcpy(block, &ctx->key[2], 2);
	memcpy_r(&block[0x38], frame_1, 8);
	SHA1_DIGEST(block, 0x40, digest);
	memcpy(frame_2, digest, 8);
	memxor(frame_2, input, 8);

	// round 3
	memcpy(block, ctx->key, 2);
	memcpy_r(&block[0x38], frame_2, 8);
	SHA1_DIGEST(block, 0x40, digest);
	memcpy(frame_3, digest, 8);
	memxor(frame_3, frame_1, 8);

	// round 4
	memcpy(block, &ctx->key[2], 2);
	memcpy_r(&block[0x38], frame_3, 8);
	SHA1_DIGEST(block, 0x40, digest);
	memcpy(frame_4, digest, 8);
	memxor(frame_4, frame_2, 8);

	// write out encrypted data
	memcpy(output, frame_4, 8);
	memcpy(&output[8], frame_3, 8);
}

// Decrypt a 16-byte block using the supplied `ctx` from `input` into `output`.
static void psp2cipher_decrypt_ecb(const psp2cipher_context *ctx,
                                   const unsigned char input[16],
                                   unsigned char output[16]) {
	unsigned char block[0x40] = {0};
	unsigned char frame_1[8];
	unsigned char frame_2[8];
	unsigned char frame_3[8];
	unsigned char frame_4[8];
	unsigned char digest[20];

	// round 1
	memcpy(block, &ctx->key[2], 2);
	memcpy_r(&block[0x38], &input[8], 8);
	SHA1_DIGEST(block, 0x40, digest);
	memcpy(frame_1, digest, 8);
	memxor(frame_1, input, 8);

	// round 2
	memcpy(block, ctx->key, 2);
	memcpy_r(&block[0x38], frame_1, 8);
	SHA1_DIGEST(block, 0x40, digest);
	memcpy(frame_2, digest, 8);
	memxor(frame_2, &input[8], 8);

	// round 3
	memcpy(block, &ctx->key[2], 2);
	memcpy_r(&block[0x38], frame_2, 8);
	SHA1_DIGEST(block, 0x40, digest);
	memcpy(frame_3, digest, 8);
	memxor(frame_3, frame_1, 8);

	// round 4
	memcpy(block, ctx->key, 2);
	memcpy_r(&block[0x38], frame_3, 8);
	SHA1_DIGEST(block, 0x40, digest);
	memcpy(frame_4, digest, 8);
	memxor(frame_4, frame_2, 8);

	// write out decrypted data
	memcpy(output, frame_3, 8);
	memcpy(&output[8], frame_4, 8);
}

int psp2cipher_crypt_ecb(const psp2cipher_context *ctx, int mode,
                         const unsigned char input[16],
                         unsigned char output[16]) {
	switch (mode) {
	case PSP2CIPHER_DECRYPT:
		psp2cipher_decrypt_ecb(ctx, input, output);
		return PSP2CIPHER_SUCCESS;
		break;

	case PSP2CIPHER_ENCRYPT:
		psp2cipher_encrypt_ecb(ctx, input, output);
		return PSP2CIPHER_SUCCESS;
		break;

	default:
		return PSP2CIPHER_ERR_BAD_MODE;
		break;
	}
}
