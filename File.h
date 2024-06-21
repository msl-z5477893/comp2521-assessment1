// Interface to the File ADT

// !!! DO NOT MODIFY THIS FILE !!!

#ifndef FILE_H
#define FILE_H

#include <stdbool.h>

#include "character.h"

typedef struct file *File;

/**
 * Opens a file for reading.
 * The file must be closed with FileClose after you have finished
 * reading from it.
 */
File FileOpenToRead(char *filename);

/**
 * Opens a file for writing.
 * If there is no file with the given name, it will be created.
 * If there is already a file with the given name, it will be truncated
 * (i.e., cleared) first.
 * The file must be closed with FileClose after you have finished
 * writing to it.
 */
File FileOpenToWrite(char *filename);

/**
 * Closes the file.
 */
void FileClose(File file);

/**
 * Reads a characater from the file and stores it, along with the
 * terminating null byte ('\0'), to the given buffer. The length of a
 * character is at most MAX_CHARACTER_LEN bytes, not including the
 * terminating null byte ('\0').
 *
 * Calling this function multiple times on a file will cause it to read
 * characters sequentially from the file. The first time this is called
 * on a file, it will read the first character, the next time it is
 * called, it will read the second character, and so on.
 *
 * Assumes that the file is open for reading.
 * Returns true if a character was successfully read, and false otherwise
 * (e.g., if the end of the file was reached).
 */
bool FileReadCharacter(File file, char buffer[]);

/**
 * Writes a string to the file.
 * The string will be written to the end of the file (i.e., after all
 * the strings that were previously written with this function). The
 * string may contain any number of characters.
 *
 * Assumes that the file is open for writing.
 */
void FileWrite(File file, char *str);

#endif
