CC=gcc
CFLAGS=-D_FILE_OFFSET_BITS=64 -g -lm

all:
	$(CC) `pkg-config fuse3 --cflags --libs` $(CFLAGS) read2.c utilit.c utilit.h
#	gcc -D_FILE_OFFSET_BITS=64 -lm read2.c utilit.c utilit.h 
#	$(CC) -o readTar1 $(CFLAGS) read2.c

