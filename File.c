// Implementation of the File ADT

// !!! DO NOT MODIFY THIS FILE !!!

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "File.h"

typedef enum {
	READ,
	WRITE,
} Mode;

struct file {
	FILE *fp;
	Mode mode;
};

File FileOpenToRead(char *filename) {
	File file = malloc(sizeof(*file));
	if (file == NULL) {
		fprintf(stderr, "error: out of memory\n");
		exit(EXIT_FAILURE);
	}

	file->fp = fopen(filename, "r");
	if (file->fp == NULL) {
		fprintf(stderr, "error: failed to open '%s' for reading\n", filename);
		exit(EXIT_FAILURE);
	}

	file->mode = READ;
	return file;
}

File FileOpenToWrite(char *filename) {
	File file = malloc(sizeof(*file));
	if (file == NULL) {
		fprintf(stderr, "error: out of memory\n");
		exit(EXIT_FAILURE);
	}

	file->fp = fopen(filename, "w");
	if (file->fp == NULL) {
		fprintf(stderr, "error: failed to open '%s' for writing\n", filename);
		exit(EXIT_FAILURE);
	}

	file->mode = WRITE;
	return file;
}

void FileClose(File file) {
	fclose(file->fp);
	free(file);
}

bool FileReadCharacter(File file, char buffer[]) {
	assert(file->mode == READ);

	int ch = fgetc(file->fp);
	if (ch == EOF) {
		return false;
	}

	ungetc(ch, file->fp);
	char byte1 = (char) ch;

	int len;
	if ((0b10000000 & byte1) == 0) {
		len = 1;
	} else if ((0b11100000 & byte1) == 0b11000000) {
		len = 2;
	} else if ((0b11110000 & byte1) == 0b11100000) {
		len = 3;
	} else if ((0b11111000 & byte1) == 0b11110000) {
		len = 4;
	} else {
		fprintf(stderr, "error: invalid character\n");
		return false;
	}

	fread(buffer, 1, len, file->fp);
	buffer[len] = '\0';
	return true;
}

void FileWrite(File file, char *str) {
	assert(file->mode == WRITE);

	fprintf(file->fp, "%s", str);
}

