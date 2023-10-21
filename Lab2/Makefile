CFLAGS = -I. -std=gnu17 -Wpedantic -Wall -Wextra -O0 -g -pipe -fno-plt -fPIC
ifeq ($(shell uname -s),Darwin)
	LDFLAGS =
else
	LDFLAGS = -lrt -Wl,-O1,--sort-common,--as-needed,-z,relro,-z,now
endif

.PHONY: all
all: rr

rr: rr.o
rr.o: rr.c stdckdint.h

.PHONY: clean
clean:
	rm -f rr.o rr
