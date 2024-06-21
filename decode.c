// Main program for decoding

// !!! DO NOT MODIFY THIS FILE !!!

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "character.h"
#include "File.h"
#include "huffman.h"

static struct huffmanTree *readHuffmanTree(char *filename);
static struct huffmanTree *readTree(FILE *fp, char buffer[]);
static struct huffmanTree *newHuffmanNode(char *character, int freq);
static void freeHuffmanTree(struct huffmanTree *t);

static char *readEncoding(char *filename);

int main(int argc, char *argv[]) {
	if (argc != 4) {
		fprintf(stderr, "usage: %s <tree filename> <encoding filename> "
		        "<output filename>\n",
		        argv[0]);
		exit(EXIT_FAILURE);
	}

	struct huffmanTree *tree = readHuffmanTree(argv[1]);
	char *encoding = readEncoding(argv[2]);

	decode(tree, encoding, argv[3]);

	freeHuffmanTree(tree);
	free(encoding);
}

static struct huffmanTree *readHuffmanTree(char *filename) {
	FILE *fp = fopen(filename, "r");
	if (fp == NULL) {
		fprintf(stderr, "error: failed to open '%s' for reading\n", filename);
		exit(EXIT_FAILURE);
	}

	char buffer[MAX_CHARACTER_LEN + 1];
	struct huffmanTree *tree = readTree(fp, buffer);
	fclose(fp);
	return tree;
}

static struct huffmanTree *readTree(FILE *fp, char buffer[]) {
	struct huffmanTree *t = newHuffmanNode(NULL, 0);

	int c = fgetc(fp);

	if (c == '(') {
		t->left = readTree(fp, buffer);
		fgetc(fp); // should always be a ','
		t->right = readTree(fp, buffer);
		fgetc(fp); // should always be a ')'
	} else {
		int i = 0;
		while (c != ',' && c != ')') {
			if (c == '\\') {
				c = fgetc(fp);
			}
			buffer[i++] = c;
			c = fgetc(fp);
		}

		ungetc(c, fp);
		buffer[i] = '\0';
		t->character = strdup(buffer);
	}

	return t;
}

static struct huffmanTree *newHuffmanNode(char *character, int freq) {
	struct huffmanTree *new = malloc(sizeof(struct huffmanTree));
	if (new == NULL) {
		fprintf(stderr, "error: out of memory\n");
		exit(EXIT_FAILURE);
	}

	new->character = character;
	new->freq = freq;
	new->left = NULL;
	new->right = NULL;
	return new;
}

static void freeHuffmanTree(struct huffmanTree *t) {
	if (t != NULL) {
		freeHuffmanTree(t->left);
		freeHuffmanTree(t->right);
		free(t->character);
		free(t);
	}
}

////////////////////////////////////////////////////////////////////////

static char *readEncoding(char *filename) {
	FILE *fp = fopen(filename, "r");
	if (fp == NULL) {
		fprintf(stderr, "error: failed to open '%s' for reading\n", filename);
		exit(EXIT_FAILURE);
	}

	fseek(fp, 0, SEEK_END);
	long pos = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	char *encoding = malloc((pos + 1) * sizeof(char));
	if (encoding == NULL) {
		fprintf(stderr, "error: out of memory\n");
		exit(EXIT_FAILURE);
	}

	if (fscanf(fp, "%s", encoding) != 1) {
		fprintf(stderr, "error: failed to read encoding\n");
		exit(EXIT_FAILURE);
	}

	fclose(fp);
	return encoding;
}

////////////////////////////////////////////////////////////////////////
