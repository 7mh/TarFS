CC=gcc
CFLAGS=-D_FILE_OFFSET_BITS=64

all: 
	$(CC) -o readTar1 $(CFLAGS) read2.c

