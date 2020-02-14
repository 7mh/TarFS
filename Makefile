CC=gcc
CFLAGS=-D_FILE_OFFSET_BITS=64

all:
	gcc -D_FILE_OFFSET_BITS=64 -lm read2.c utilit.c utilit.h 
#	$(CC) -o readTar1 $(CFLAGS) read2.c

