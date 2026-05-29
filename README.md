# tntpsp_keygen

tntpsp_keygen is an application that reimplements the algorithms present in
Taiko no Tatsujin Portable 1/2 used to create the DLC download keys used to
download and access downloadable song data from the official website.
Specifically, these keys are those present in the `key` parameter of the URL
present in the `download.htm` file created by the game on the inserted Memory
Stick after accessing the appropriate menu option. This application can also
decrypt DLC keys back into their respective OpenPSID's.

# Build instructions

## Linux/MinGW

To build using built-in libraries for AES/SHA-1:
```
$ make
```
This application can optionally be built using the Mbed TLS implementations of
AES/SHA-1, if the appropriate headers are installed, which may produce a
slightly faster/smaller binary. To build using Mbed TLS:
```
$ make USE_MBEDTLS=1
```

# Usage

## Linux

### Synopsis

```
$ ./tntpsp_keygen <e|d> <1|2> <data> | [-h|-u|-v]
```
Detailed usage information can be found by running tntpsp_keygen with the -h
flag.

### Examples

To produce a key for Portable 1 and your PSP's OpenPSID:
```
$ ./tntpsp_keygen e 1 00112233445566778899aabbccddeeff
e592829e79ef488d0a4e86c92e7c0dc10c76
```
To decrypt a DLC key from Portable 2 back into an OpenPSID:
```
$ ./tntpsp_keygen d 2 00112233445566778899aabbccddeeff0011
6237063644f40fc56b80fbd9661dec57
```

## Windows

All commands are the same, just replace `./tntpsp_keygen` with `tntpsp_keygen.exe`.

# Credits

tntpsp_keygen (c) 2026 enderghast13

This software is licensed for use under [Zero-Clause BSD](https://spdx.org/licenses/0BSD.html).

[Tiny AES](https://github.com/kokke/tiny-AES-c) by kokke and contributors is
licensed for use under [The Unlicense](https://spdx.org/licenses/Unlicense.html)/is
in the public domain.

[sha1.c and sha1.h](https://github.com/clibs/sha1) by Steve Reid and contributors
are in the public domain.

[Mbed TLS](https://www.trustedfirmware.org/projects/mbed-tls/) (c) The Mbed TLS
Contributors is licensed for use under either [Apache 2.0](https://spdx.org/licenses/Apache-2.0.html)
or [GPL 2.0 or later](https://spdx.org/licenses/GPL-2.0-or-later.html) at the
user's discretion.
