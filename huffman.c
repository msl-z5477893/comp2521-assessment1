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
bool is_leaf(struct huffmanTree *);

// Task 1
void decode(struct huffmanTree *tree, char *encoding, char *outputFilename) {
        File file = FileOpenToWrite(outputFilename);
        size_t encoding_ptr = 0;
        struct huffmanTree *root = tree;
        struct huffmanTree *tree_ptr = tree;
        while (true) {
                if (is_leaf(tree_ptr)) {
                        FileWrite(file, tree_ptr->character);
                        tree_ptr = root;
                        continue;
                }

                if (encoding[encoding_ptr] == '0') {
                        tree_ptr = tree_ptr->left;
                } else if (encoding[encoding_ptr] == '1') {
                        tree_ptr = tree_ptr->right;
                } else {
                        break;
                }
                encoding++;
        }
        FileClose(file);
}

bool is_leaf(struct huffmanTree *tree) {
        return tree->left == NULL && tree->right == NULL;
}

// Task 3
struct huffmanTree *createHuffmanTree(char *inputFilename) { return NULL; }

// Task 4
char *encode(struct huffmanTree *tree, char *inputFilename) { return NULL; }

