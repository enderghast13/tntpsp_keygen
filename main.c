// SPDX-License-Identifier: 0BSD
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#if (defined(USE_MBEDTLS) && (USE_MBEDTLS == 1))
  #include <mbedtls/aes.h>
#else
  #include "aes.h"
#endif
#include "psp2cipher.h"

#define u8 uint8_t

// application metadata
static const char TITLE[]   = "tntpsp_keygen";
static const char VERSION[] = "1.0.1";
static const char YEAR[]    = "2026";
static const char AUTHOR[]  = "enderghast13";

// command line parser error strings
static const char E_ARGC[] = "incorrect number of arguments (must be 3)";
static const char E_ARG1[] = "arg 1: must be either 'e' or 'd'";
static const char E_ARG2[] = "arg 2: must be either '1' or '2'";
static const char E_ARG3[] = "arg 3: wrong format (must be 16/18 bytes hexadecimal)";

// crypto mode
enum MODES {
	DECRYPT,
	ENCRYPT,
};
// game generation
enum GENERATIONS {
	PSP1,
	PSP2,
};

// struct to contain all command line arguments
typedef struct {
	bool usage;
	bool version;
	int mode;
	int gen;
	u8 data[18];
} Arguments;

// crypto keys for PSP1
static const u8 KEY_1_A[] = {
	0x44, 0x27, 0xD2, 0xC5, 0xB4, 0x5A, 0x05, 0xA9,
	0x82, 0x28, 0x60, 0x59, 0x28, 0xAC, 0xE7, 0x05,
	0xB8, 0xC0, 0xB1, 0x93, 0x87, 0x0C, 0x4D, 0xC6,
};
static const u8 KEY_1_B[] = {
	0x50, 0x28, 0x11, 0x69, 0x42, 0x81, 0xC0, 0x6F,
	0x36, 0x1B, 0x56, 0xE8, 0xE4, 0x0A, 0x35, 0x87,
	0x4E, 0xAF, 0xD4, 0x88, 0x14, 0x4B, 0x2D, 0x9F,
};
static const u8 SALT_1[]  = {0x81, 0x9B};

// crypto keys for PSP2
static const u8 KEY_2_A[] = {0xC6, 0xC0, 0x4B, 0x97};
static const u8 KEY_2_B[] = {0xB5, 0xFB, 0x94, 0x5A};
static const u8 SALT_2[]  = {0x00, 0x3B};


// Print an error message. Returns the `fprintf()` status.
static int eprint(const char *restrict prog, const char *restrict msg) {
	return fprintf(stderr, "%s: error: %s\n", prog, msg);
}

// Print usage information.
static void usage(const char *restrict prog) {
	printf(
		"Usage: %s <e|d> <1|2> <data> | [-h|-u|-v]\n"
		"Taiko no Tatsujin Portable 1/2 DLC keygen\n"
		"\n"
		"arguments:\n"
		"  <e|d>          encrypt (generate) or decrypt key\n"
		"  <1|2>          game to generate/decrypt key for\n"
		"  <data>         OpenPSID / DLC key in hex\n"
		"\n"
		"options:\n"
		"  -h, --help     show this help message\n"
		"  -u, --usage    same as -h\n"
		"  -v, --version  show version information\n\n",
		prog
	);
}

// Print version and license information.
static void version(void) {
	printf(
		"%s v%s - Taiko no Tatsujin Portable 1/2 DLC keygen\n"
		"Copyright (c) %s by %s\n"
		"\n"
		"Permission to use, copy, modify, and/or distribute this software for any\n"
		"purpose with or without fee is hereby granted.\n"
		"\n"
		"THE SOFTWARE IS PROVIDED \"AS IS\" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH\n"
		"REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND\n"
		"FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,\n"
		"INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM\n"
		"LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR\n"
		"OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR\n"
		"PERFORMANCE OF THIS SOFTWARE.\n\n",
		TITLE, VERSION, YEAR, AUTHOR
	);
}

// Parse command line arguments into `args`.
static int parse_args(Arguments *restrict args, int argc, const char *restrict argv[]) {
	// initialize all member variables except data
	args->usage   = false;
	args->version = false;
	args->mode    = DECRYPT;
	args->gen     = PSP1;

	if (argc == 4) {
		// parsing all arguments

		// mode
		if (strcmp(argv[1], "d") == 0) {
			args->mode = DECRYPT;
		} else if (strcmp(argv[1], "e") == 0) {
			args->mode = ENCRYPT;
		} else {
			eprint(argv[0], E_ARG1);
			return -1;
		}

		// generation
		if (strcmp(argv[2], "1") == 0) {
			args->gen = PSP1;
		} else if (strcmp(argv[2], "2") == 0) {
			args->gen = PSP2;
		} else {
			eprint(argv[0], E_ARG2);
			return -1;
		}

		// data
		switch (args->mode) {
			int n;

		case DECRYPT:
			// read DLC key
			if (
				strlen(argv[3]) != 36  // check length
				|| sscanf(
					argv[3],
					"%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx"
					"%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%n",
					&args->data[0], &args->data[1], &args->data[2],
					&args->data[3], &args->data[4], &args->data[5],
					&args->data[6], &args->data[7], &args->data[8],
					&args->data[9], &args->data[10], &args->data[11],
					&args->data[12], &args->data[13], &args->data[14],
					&args->data[15], &args->data[16], &args->data[17],
					&n
				) != 18  // parse into args->data, check if incorrect
				|| n != 36  // make sure we're at the end
				            // (in case the very last character was bad)
			) {
				eprint(argv[0], E_ARG3);
				return -1;
			}
			break;

		case ENCRYPT:
			// read OpenPSID
			if (
				strlen(argv[3]) != 32  // check length
				|| sscanf(
					argv[3],
					"%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx"
					"%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%n",
					&args->data[0], &args->data[1], &args->data[2],
					&args->data[3], &args->data[4], &args->data[5],
					&args->data[6], &args->data[7], &args->data[8],
					&args->data[9], &args->data[10], &args->data[11],
					&args->data[12], &args->data[13], &args->data[14],
					&args->data[15], &n
				) != 16  // parse into args->data, check if incorrect
				|| n != 32  // make sure we're at the end
				            // (in case the very last character was bad)
			) {
				eprint(argv[0], E_ARG3);
				return -1;
			}
			break;
		}

	} else if (argc == 2) {
		// information options

		if (  // usage
			strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0
			|| strcmp(argv[1], "-u") == 0 || strcmp(argv[1], "--usage") == 0
		) {
			args->usage = true;
		} else if (  // version
			strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--version") == 0
		) {
			args->version = true;
		} else {  // bad argument
			if (strcmp(argv[1], "d") == 0 || strcmp(argv[1], "e") == 0) {
				eprint(argv[0], E_ARGC);
			} else {
				eprint(argv[0], E_ARG1);
			}
			return -1;
		}

	} else {
		// bad number of arguments
		eprint(argv[0], E_ARGC);
		return -1;
	}

	return 0;
}

// Generate (encrypt) a DLC key into `dlc_key` from the supplied OpenPSID.
// Returns 0 on success, some other number on error.
static int gen_dlc_key(int gen, const u8 openpsid[restrict 16],
                       u8 dlc_key[restrict 18]) {
	switch (gen) {
	case PSP1:
		// using AES
#if (defined(USE_MBEDTLS) && (USE_MBEDTLS == 1))
		mbedtls_aes_context aes;

		mbedtls_aes_init(&aes);
		mbedtls_aes_setkey_enc(&aes, KEY_1_A, 192);
		mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_ENCRYPT, openpsid, dlc_key);
		memcpy(&dlc_key[16], SALT_1, 2);

		mbedtls_aes_setkey_enc(&aes, KEY_1_B, 192);
		mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_ENCRYPT, &dlc_key[2], &dlc_key[2]);

		mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_ENCRYPT, dlc_key, dlc_key);

		mbedtls_aes_free(&aes);
#else  // Tiny AES
		struct AES_ctx aes;

		AES_init_ctx(&aes, KEY_1_A);
		memcpy(dlc_key, openpsid, 16);
		AES_ECB_encrypt(&aes, dlc_key);
		memcpy(&dlc_key[16], SALT_1, 2);

		AES_init_ctx(&aes, KEY_1_B);
		AES_ECB_encrypt(&aes, &dlc_key[2]);

		AES_ECB_encrypt(&aes, dlc_key);
#endif  // #if (defined(USE_MBEDTLS) && (USE_MBEDTLS == 1))
		break;

	case PSP2:
		// using psp2cipher
		psp2cipher_context ctx;

		psp2cipher_setkey(&ctx, KEY_2_A);
		psp2cipher_crypt_ecb(&ctx, PSP2CIPHER_ENCRYPT, openpsid, dlc_key);
		memcpy(&dlc_key[16], SALT_2, 2);

		psp2cipher_setkey(&ctx, KEY_2_B);
		psp2cipher_crypt_ecb(&ctx, PSP2CIPHER_ENCRYPT, &dlc_key[2], &dlc_key[2]);

		psp2cipher_crypt_ecb(&ctx, PSP2CIPHER_ENCRYPT, dlc_key, dlc_key);
		break;

	default:
		// invalid mode
		return -1;
		break;
	}

	return 0;
}

// Decrypt the supplied DLC key into an OpenPSID, written into `openpsid`.
// Retuns 0 on success, some other number on error.
static int decrypt_dlc_key(int gen, const u8 dlc_key[restrict 18],
                           u8 openpsid[restrict 16]) {
	u8 buf[18];
	memcpy(buf, dlc_key, 18);

	switch (gen) {
	case PSP1:
		// using AES
#if (defined(USE_MBEDTLS) && (USE_MBEDTLS == 1))
		mbedtls_aes_context aes;

		mbedtls_aes_init(&aes);
		mbedtls_aes_setkey_dec(&aes, KEY_1_B, 192);
		mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_DECRYPT, buf, buf);

		mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_DECRYPT, &buf[2], &buf[2]);

		mbedtls_aes_setkey_dec(&aes, KEY_1_A, 192);
		mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_DECRYPT, buf, openpsid);

		mbedtls_aes_free(&aes);
#else  // Tiny AES
		struct AES_ctx aes;

		AES_init_ctx(&aes, KEY_1_B);
		AES_ECB_decrypt(&aes, buf);

		AES_ECB_decrypt(&aes, &buf[2]);

		AES_init_ctx(&aes, KEY_1_A);
		AES_ECB_decrypt(&aes, buf);
		memcpy(openpsid, buf, 16);
#endif  // #if (defined(USE_MBEDTLS) && (USE_MBEDTLS == 1))
		break;

	case PSP2:
		// using psp2cipher
		psp2cipher_context ctx;

		psp2cipher_setkey(&ctx, KEY_2_B);
		psp2cipher_crypt_ecb(&ctx, PSP2CIPHER_DECRYPT, dlc_key, buf);

		psp2cipher_crypt_ecb(&ctx, PSP2CIPHER_DECRYPT, &buf[2], &buf[2]);

		psp2cipher_setkey(&ctx, KEY_2_A);
		psp2cipher_crypt_ecb(&ctx, PSP2CIPHER_DECRYPT, buf, openpsid);
		break;

	default:
		// invalid mode
		return -1;
		break;
	}

	return 0;
}

int main(int argc, char *argv[]) {
	if (argc == 0) {
		fputs("error: not enough arguments, exiting", stderr);
		return 1;
	}

	Arguments args;
	if (parse_args(&args, argc, (const char **)argv) != 0) {
		return 1;
	}

	if (args.usage) {
		usage(argv[0]);
		return 0;
	}
	if (args.version) {
		version();
		return 0;
	}

	switch (args.mode) {
	case DECRYPT:
		u8 openpsid[16];

		decrypt_dlc_key(args.gen, args.data, openpsid);

		printf(
			"%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx"
			"%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx\n",
			openpsid[0], openpsid[1], openpsid[2], openpsid[3], openpsid[4],
			openpsid[5], openpsid[6], openpsid[7], openpsid[8], openpsid[9],
			openpsid[10], openpsid[11], openpsid[12], openpsid[13],
			openpsid[14], openpsid[15]
		);
		break;

	case ENCRYPT:
		u8 dlc_key[18];

		gen_dlc_key(args.gen, args.data, dlc_key);

		printf(
			"%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx"
			"%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx\n",
			dlc_key[0], dlc_key[1], dlc_key[2], dlc_key[3], dlc_key[4],
			dlc_key[5], dlc_key[6], dlc_key[7], dlc_key[8], dlc_key[9],
			dlc_key[10], dlc_key[11], dlc_key[12], dlc_key[13], dlc_key[14],
			dlc_key[15], dlc_key[16], dlc_key[17]
		);
		break;
	}
}
