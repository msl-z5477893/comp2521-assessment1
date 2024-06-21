# COMP2521 - Assignment 1

# !!! DO NOT MODIFY THIS FILE !!!

CC = clang
CFLAGS = -Wall -Wvla -Werror -g

########################################################################

.PHONY: asan msan nosan

asan: CFLAGS += -fsanitize=address,leak,undefined
asan: all

msan: CFLAGS += -fsanitize=memory,undefined -fsanitize-memory-track-origins
msan: all

nosan: all

########################################################################

.PHONY: all
all: encode decode testCounter

encode: encode.c huffman.c Counter.c File.c
	$(CC) $(CFLAGS) -o encode encode.c huffman.c Counter.c File.c

decode: decode.c huffman.c Counter.c File.c
	$(CC) $(CFLAGS) -o decode decode.c huffman.c Counter.c File.c

testCounter: testCounter.c Counter.c
	$(CC) $(CFLAGS) -o testCounter testCounter.c Counter.c

.PHONY: clean
clean:
	rm -f encode decode testCounter
