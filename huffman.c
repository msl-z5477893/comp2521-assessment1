// Implementation of the Huffman module

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Counter.h"
#include "File.h"
#include "character.h"
#include "huffman.h"

// CUSTOM HELPER FUNCTIONS
bool isLeaf(struct huffmanTree *);
char *getUtf8Char(File);

// Task 1
// decode huffman data given tree and encoding

// current implementation involves moving a pointer to the tree
// using the encoding until it points on a leaf, of which
// we print the character in the tree node then go back to the root
// node.
// Potential optimisations available.
void decode(struct huffmanTree *tree, char *encoding, char *outputFilename) {
        File file = FileOpenToWrite(outputFilename);
        size_t encodingPtr = 0;
        struct huffmanTree *root = tree;
        struct huffmanTree *treePtr = tree;
        while (true) {
                if (isLeaf(treePtr)) {
                        FileWrite(file, treePtr->character);
                        treePtr = root;
                        continue;
                }

                if (encoding[encodingPtr] == '0') {
                        treePtr = treePtr->left;
                } else if (encoding[encodingPtr] == '1') {
                        treePtr = treePtr->right;
                } else {
                        break;
                }
                encoding++;
        }
        FileClose(file);
}

// check if current tree node is a leaf
bool isLeaf(struct huffmanTree *tree) {
        return tree->left == NULL && tree->right == NULL;
}

// Task 3
struct huffmanTree *createHuffmanTree(char *inputFilename) {
        Counter charCount = CounterNew();
        File fstream = FileOpenToRead(inputFilename);
        FileClose(fstream);
        CounterFree(charCount);
        return NULL;
}

// a character from a file, this function accounts for utf8
// representation.
char *getUtf8Char(File fstream) { return NULL; }

// Task 4
char *encode(struct huffmanTree *tree, char *inputFilename) { return NULL; }

