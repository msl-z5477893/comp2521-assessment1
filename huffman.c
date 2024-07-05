// Implementation of the Huffman module
// Completed by Michael Stephen Lape (z5477893@ad.unsw.edu.au)

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Counter.h"
#include "File.h"
#include "character.h"
#include "huffman.h"

// custom helper functions and data structures.

// a linked list for storing multiple huffman trees
// the list is sorted in ascending order when the tree is added.
// this is used to create the full huffman tree.
struct huffmanTreeArena {
    struct huffmanTreeArenaNode *head;
    int size;
};

struct huffmanTreeArenaNode {
    struct huffmanTreeArenaNode *next;
    struct huffmanTree *tree;
};

// a unit structure relating a character to its corresponding encoding
struct charEncoding {
    char *character;
    char *code;
};

// linked-list representation of a code prefix.
typedef struct prefixNode {
    char dir;
    struct prefixNode *next;
} PrefixNode;

struct prefixPath {
    PrefixNode *head;
    PrefixNode *tail;
    char *character;
    int length;
};

// for very large strings
struct buffer {
    char *str;
    unsigned int capacity;
    unsigned int charCount;
};

// misc functions.
static bool isLeaf(struct huffmanTree *);
static int leafCount(struct huffmanTree *);
static char *getUtf8Char(File);

// HuffmanTreeArena functions
static struct huffmanTree *huffmanTreeFromItem(struct item);
static struct huffmanTreeArena *huffmanTreeArenaNew(void);
static void huffmanTreeArenaFree(struct huffmanTreeArena *);
static struct huffmanTreeArenaNode *
huffmanTreeArenaPop(struct huffmanTreeArena *);
static bool huffmanTreeArenaAdd(struct huffmanTreeArena *,
                                struct huffmanTree *);
static bool huffmanTreeArenaAssertOrder(struct huffmanTreeArena *);

// prefix path functions
static struct prefixPath *convertBranchToPath(struct huffmanTree *);

// buffer function
static struct buffer *initBuffer(size_t size);
static void insertToBuffer(char *chars);

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
static bool isLeaf(struct huffmanTree *tree) {
    return tree->left == NULL && tree->right == NULL;
}

// Task 3
struct huffmanTree *createHuffmanTree(char *inputFilename) {
    Counter charCount = CounterNew();
    File fstream = FileOpenToRead(inputFilename);
    int distinctCharCount = 0;

    // generate count tree.
    char charUtf8[MAX_CHARACTER_LEN + 1];
    while (FileReadCharacter(fstream, charUtf8)) {
        CounterAdd(charCount, charUtf8);
    }

    // create an array of huffman trees, each containing one character and a
    // frequncy.
    struct item *fileCharData = CounterItems(charCount, &distinctCharCount);
    struct huffmanTreeArena *treeArena = huffmanTreeArenaNew();
    printf("Start proessing item array...\n");
    for (int index = 0; index < distinctCharCount; index++) {
        // printf("proccessing character \"%s\"...\n",
        //        fileCharData[index].character);
        struct huffmanTree *tree = huffmanTreeFromItem(fileCharData[index]);
        huffmanTreeArenaAdd(treeArena, tree);
        if (!huffmanTreeArenaAssertOrder(treeArena)) {
            printf("WARNING: tree went out of order!\n");
        }
    }

    while (treeArena->size != 1) {
        struct huffmanTree *newBiggerTree = malloc(sizeof(struct huffmanTree));
        struct huffmanTreeArenaNode *arenaNode1 =
            huffmanTreeArenaPop(treeArena);
        struct huffmanTreeArenaNode *arenaNode2 =
            huffmanTreeArenaPop(treeArena);
        struct huffmanTree *lowestFirst = arenaNode1->tree;
        struct huffmanTree *lowestSecond = arenaNode2->tree;

        newBiggerTree->character = NULL;
        newBiggerTree->freq = lowestFirst->freq + lowestSecond->freq;
        newBiggerTree->left = lowestFirst;
        newBiggerTree->right = lowestSecond;

        huffmanTreeArenaAdd(treeArena, newBiggerTree);
        free(arenaNode1);
        free(arenaNode2);
        if (!huffmanTreeArenaAssertOrder(treeArena)) {
            printf("WARNING: tree went out of order!\n");
        }
    }
    printf("final size of arena: %d\n.", treeArena->size);
    struct huffmanTreeArenaNode *lastNode = huffmanTreeArenaPop(treeArena);
    struct huffmanTree *finalTree = lastNode->tree;

    free(lastNode);
    huffmanTreeArenaFree(treeArena);
    free(fileCharData);
    FileClose(fstream);
    CounterFree(charCount);
    return finalTree;
}

// helper functions

// create a leaf huffmanTree from item struct
static struct huffmanTree *huffmanTreeFromItem(struct item item) {
    struct huffmanTree *newTree = malloc(sizeof(struct huffmanTree));
    newTree->character = malloc(sizeof(char) * (MAX_CHARACTER_LEN + 1));
    strncpy(newTree->character, item.character, MAX_CHARACTER_LEN + 1);
    newTree->freq = item.freq;
    newTree->left = NULL;
    newTree->right = NULL;
    return newTree;
}

// initialise a huffmanTreeArena
static struct huffmanTreeArena *huffmanTreeArenaNew() {
    struct huffmanTreeArena *arena = malloc(sizeof(struct huffmanTreeArena));
    arena->head = NULL;
    arena->size = 0;
    return arena;
}

// free arena
// this must be done once the full tree is completed
// asserts that there are no nodes inside.
static void huffmanTreeArenaFree(struct huffmanTreeArena *arena) {
    assert(arena->size == 0);
    free(arena->head);
    free(arena);
}

// add to huffman tree arena
// tree must be inserted in ascending order
// should always return true.
static bool huffmanTreeArenaAdd(struct huffmanTreeArena *arena,
                                struct huffmanTree *tree) {
    // memory initialisation
    struct huffmanTreeArenaNode *newNode =
        malloc(sizeof(struct huffmanTreeArenaNode));

    // assign objects in memory
    newNode->tree = tree;
    newNode->next = NULL;

    // base case: arena has no nodes.
    if (arena->size == 0) {
        arena->head = newNode;
        arena->size++;
        return true;
    }

    // case: letter frequency is smallest.
    if (newNode->tree->freq < arena->head->tree->freq) {
        newNode->next = arena->head;
        arena->head = newNode;
        arena->size++;
        return true;
    }

    // case: letter frequency is a middle value.
    struct huffmanTreeArenaNode *arenaNode = arena->head;
    while (arenaNode->next != NULL) {
        if (newNode->tree->freq < arenaNode->next->tree->freq) {
            newNode->next = arenaNode->next;
            arenaNode->next = newNode;
            arena->size++;
            return true;
        }
        arenaNode = arenaNode->next;
    }

    // case: letter frequency is largest.
    arenaNode->next = newNode;
    arena->size++;
    return true;
}

// remove and return the first node in the arena
// popped nodes must be freed individually.
static struct huffmanTreeArenaNode *
huffmanTreeArenaPop(struct huffmanTreeArena *arena) {
    struct huffmanTreeArenaNode *popped = arena->head;
    arena->head = arena->head->next;
    arena->size--;
    return popped;
}

// checks that the trees in the arena is still in ascending order.
static bool huffmanTreeArenaAssertOrder(struct huffmanTreeArena *arena) {
    struct huffmanTreeArenaNode *arenaNode = arena->head;
    while (arenaNode->next != NULL) {
        if (arenaNode->tree->freq > arenaNode->next->tree->freq) {
            return false;
        }
        arenaNode = arenaNode->next;
    }
    return true;
}

// Task 4
char *encode(struct huffmanTree *tree, char *inputFilename) {
    // get all possible character encodings
    // TODO: fix this somehow
    File fstream = FileOpenToRead(inputFilename);
    int uniqueLetterCount = leafCount(tree);
    struct prefixPath *paths =
        malloc(sizeof(struct prefixPath) * uniqueLetterCount);
    for (int i = 0; i < uniqueLetterCount; i++) {
        paths[i] = *convertBranchToPath(tree);
        if (&paths[i] == NULL) {
            printf("THAT SHOULD NOT BE NULL!\n");
        }
    }

    // somehow encode the entire text in file onto one massive string.
    free(paths);
    FileClose(fstream);
    return NULL;
}

// get the number of leaves in the tree
static int leafCount(struct huffmanTree *tree) {
    if (tree == NULL) {
        return 0;
    }
    int countedLeaves = leafCount(tree->left) + leafCount(tree->right);
    if (countedLeaves == 0) {
        return countedLeaves + 1;
    }
    return countedLeaves;
}
