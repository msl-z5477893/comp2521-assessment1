// Interface to Huffman module

// !!! DO NOT MODIFY THIS FILE !!!

#ifndef HUFFMAN_H
#define HUFFMAN_H

struct huffmanTree {
	char *character; // should be NULL unless the node is a leaf
	int freq;
	struct huffmanTree *left;
	struct huffmanTree *right;
};

// Part 1
void decode(struct huffmanTree *tree, char *encoding, char *outputFilename);

// Part 3
struct huffmanTree *createHuffmanTree(char *inputFilename);

// Part 4
char *encode(struct huffmanTree *tree, char *inputFilename);

#endif
