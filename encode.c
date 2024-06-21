// Main program for encoding

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

static void writeHuffmanTree(struct huffmanTree *tree, char *filename);
static void writeTree(struct huffmanTree *t, FILE *fp);
static void writeEncoding(char *encoding, char *filename);

void showHuffmanTree(struct huffmanTree *t);
static void freeHuffmanTree(struct huffmanTree *t);

int main(int argc, char *argv[]) {
	if (argc != 3 && argc != 4) {
		fprintf(stderr,
		        "usage: %s <input filename> <tree filename> "
		        "[encoding filename]\n",
		        argv[0]);
		exit(EXIT_FAILURE);
	}

	if (argc == 3) {
		struct huffmanTree *tree = createHuffmanTree(argv[1]);
		if (tree == NULL) {
			fprintf(stderr,
			        "error: createHuffmanTree returned an empty tree\n");
			exit(EXIT_FAILURE);
		}
		writeHuffmanTree(tree, argv[2]);
		freeHuffmanTree(tree);
	} else {
		struct huffmanTree *tree = readHuffmanTree(argv[2]);
		char *encoding = encode(tree, argv[1]);
		writeEncoding(encoding, argv[3]);
		free(encoding);
		freeHuffmanTree(tree);
	}
}

////////////////////////////////////////////////////////////////////////

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

static void writeHuffmanTree(struct huffmanTree *tree, char *filename) {
	FILE *fp = fopen(filename, "w");
	if (fp == NULL) {
		fprintf(stderr, "error: failed to open '%s' for writing\n", filename);
		exit(EXIT_FAILURE);
	}

	writeTree(tree, fp);

	fclose(fp);
}

static void writeTree(struct huffmanTree *t, FILE *fp) {
	if (t->left == NULL && t->right == NULL) {
		for (int i = 0; t->character[i] != '\0'; i++) {
			char c = t->character[i];
			if (c == '(' || c == ')' || c == ',' || c == '\\') {
				fputc('\\', fp);
			}
			fputc(c, fp);
		}
	} else if (t->left == NULL || t->right == NULL) {
		fprintf(
			stderr,
			"error: found node with exactly one child\n"
			"       all nodes should have zero or two children\n"
		);
		exit(EXIT_FAILURE);
	} else {
		fputc('(', fp);
		writeTree(t->left, fp);
		fputc(',', fp);
		writeTree(t->right, fp);
		fputc(')', fp);
	}
}

////////////////////////////////////////////////////////////////////////

static void writeEncoding(char *encoding, char *filename) {
	FILE *fp = fopen(filename, "w");
	if (fp == NULL) {
		fprintf(stderr, "error: failed to open '%s' for writing\n", filename);
		exit(EXIT_FAILURE);
	}

	fprintf(fp, "%s", encoding);
	fclose(fp);
}

////////////////////////////////////////////////////////////////////////
