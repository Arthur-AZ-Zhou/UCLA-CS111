ifeq ($(shell uname -s),Darwin)
	CFLAGS = -std=gnu17 -pthread -Wall -O0 -pipe -fno-plt -fPIC
	LDFLAGS =
else
	CFLAGS = -std=gnu17 -pthread -Wall -O0 -pipe -fno-plt -fPIC
	LDFLAGS = -lrt -pthread -Wl,-O1,--sort-common,--as-needed,-z,relro,-z,now
endif

.PHONY: all
all: ext2-create

ext2-create: ext2-create.o

.PHONY: clean
clean:
	rm -f ext2-create.o ext2-create
	rm -f *.img
