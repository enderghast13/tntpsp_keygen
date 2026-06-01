# SPDX-License-Identifier: 0BSD
TITLE   := tntpsp_keygen

OBJECTS := main.o psp2cipher.o

OPT     := -O3
CFLAGS  := -std=c23 -Wall -Wextra -pedantic -Werror $(OPT)
LDFLAGS :=

ifeq ($(USE_MBEDTLS),1)
	CFLAGS  += -DUSE_MBEDTLS
	LDFLAGS += -lmbedcrypto
else
	OBJECTS += aes.o sha1.o
endif


all: $(TITLE)

$(TITLE): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

clean:
	rm -f $(OBJECTS) $(TITLE) $(TITLE).exe

check: $(TITLE)
	./test.sh

.PHONY: all clean check
