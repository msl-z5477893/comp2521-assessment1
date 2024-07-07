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

#define ENCODING_0 '0'
#define ENCODING_1 '1'
#define ENCODING_END '\0'

// custom helper functions and data structures.

// Task 3 Data Structures.

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

// Task 4 Data Structures.

// a unit structure relating a character to its corresponding encoding
struct charEncoding {
    char *character;
    char *code;
};

// linked-list representation of a code prefix.
typedef struct prefixNode {
    char dir;
    char *finalChar;
    struct prefixNode *next;
} PrefixNode;

struct prefixPath {
    PrefixNode *head;
    PrefixNode *tail;
    int length;
};

// a unit struct for performing level-order traversal of tree
struct huffmanCrawler {
    struct prefixPath *address;
    struct huffmanTree *tree;
    struct huffmanCrawler *next;
};

// a queue for traversing a huffman tree
struct huffmanTraverser {
    struct huffmanCrawler *head;
    struct huffmanCrawler *tail;
};

// for very large strings
struct buffer {
    char *str;
    unsigned int capacity;
    unsigned int charCount;
};

// the struct used for encoding in task 4.
struct encoder {
    int charSum;
    unsigned int encodingLength;
    char *encoding;
};

// misc functions.
static bool isLeaf(struct huffmanTree *);
static int leafCount(struct huffmanTree *);
// static char *getUtf8Char(File);
static int characterSum(char *);
static int treeHeight(struct huffmanTree *);

// HuffmanTreeArena functions
static struct huffmanTree *huffmanTreeFromItem(struct item);
static struct huffmanTreeArena *huffmanTreeArenaNew(void);
static void huffmanTreeArenaFree(struct huffmanTreeArena *);
static struct huffmanTreeArenaNode *
huffmanTreeArenaPop(struct huffmanTreeArena *);
static bool huffmanTreeArenaAdd(struct huffmanTreeArena *,
                                struct huffmanTree *);

// prefix path functions
static struct prefixPath *prefixPathNew(void);
static struct prefixPath *prefixPathCopy(struct prefixPath *orig);
static struct encoder *prefixPathEncoding(struct prefixPath *);
static void prefixPathFree(struct prefixPath *);
static void prefixPathRecord(struct prefixPath *, char, char *);

// huffmanCrawler functions
static struct huffmanCrawler *crawlerCopy(struct huffmanCrawler *crawler);
static struct huffmanCrawler *crawlLeft(struct huffmanCrawler *crawler);
static struct huffmanCrawler *crawlRight(struct huffmanCrawler *crawler);

// huffmanTraverser functions
static struct huffmanTraverser *huffmanTraverserInit(struct huffmanTree *tree);
static struct encoder *huffmanTraverserPerform(struct huffmanTraverser *trav);
static struct huffmanCrawler *
huffmanTraverserPop(struct huffmanTraverser *trav);
static void huffmanTraverserAdd(struct huffmanTraverser *trav,
                                struct huffmanCrawler *crawler);
static void huffmanTraverserDeinit(struct huffmanTraverser *);

// buffer function
static struct buffer *bufferInit(size_t size);
static char *bufferGetStr(struct buffer *);
static void bufferInsert(struct buffer *, char *chars, unsigned int len);
static void bufferFree(struct buffer *);

// Task 1
// decode huffman data given tree and encoding
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
    for (int index = 0; index < distinctCharCount; index++) {
        struct huffmanTree *tree = huffmanTreeFromItem(fileCharData[index]);
        huffmanTreeArenaAdd(treeArena, tree);
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
    }
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

// Task 4
char *encode(struct huffmanTree *tree, char *inputFilename) {
    // TODO: fix this somehow
    // initial data
    File fstream = FileOpenToRead(inputFilename);
    char charBuf[MAX_CHARACTER_LEN + 1];
    int symCount = leafCount(tree);
    int maxEncLen = treeHeight(tree) + 1;
    struct buffer *buf =
        bufferInit(symCount * symCount * maxEncLen * maxEncLen);
    struct huffmanTraverser *trav = huffmanTraverserInit(tree);
    struct encoder *encoders = malloc(sizeof(struct encoder) * symCount);

    // generate encoding tree from huffman tree
    for (int ix = 0; ix < symCount; ix++) {
        struct encoder *enc = huffmanTraverserPerform(trav);
        encoders[ix].charSum = enc->charSum;
        encoders[ix].encodingLength = enc->encodingLength;
        encoders[ix].encoding = malloc(encoders[ix].encodingLength);
        strncpy(encoders[ix].encoding, enc->encoding, enc->encodingLength);
        free(enc->encoding);
        free(enc);
    }

    // somehow encode the entire text in file onto one massive string.
    while (FileReadCharacter(fstream, charBuf)) {
        int charKey = characterSum(charBuf);
        int ix = 0;
        while (encoders[ix].charSum != charKey) {
            ix++;
        }
        bufferInsert(buf, encoders[ix].encoding,
                     encoders[ix].encodingLength - 1);
    }
    // bufferInsert(buf, "\n", strlen("\n") + 1);

    char *result = bufferGetStr(buf);

    // cleanup :)
    for (int ix = 0; ix < symCount; ix++) {
        free(encoders[ix].encoding);
    }
    free(encoders);
    huffmanTraverserDeinit(trav);
    bufferFree(buf);
    FileClose(fstream);
    return result;
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

// get tree height
static int treeHeight(struct huffmanTree *tree) {
    if (tree == NULL) {
        return 0;
    } else {
        int heightLeft = treeHeight(tree->left);
        int heightRight = treeHeight(tree->right);
        int max;
        if (heightLeft > heightRight) {
            max = heightLeft;
        } else {
            max = heightRight;
        }
        return max + 1;
    }
}

// get the sum of the bytes of the character
// not perfect, prone to collisions, fails to encode wonderland
static int characterSum(char *str) {
    unsigned int ix = 0;
    int sum = 0;
    while (str[ix] != '\0') {
        sum += str[ix] * str[ix];
        ix++;
    }
    return sum;
}

// implementation of huffmanTraverser functions

// initialises a traverser for a specified tree
static struct huffmanTraverser *huffmanTraverserInit(struct huffmanTree *tree) {
    struct huffmanTraverser *trav = malloc(sizeof(struct huffmanTraverser));
    struct huffmanCrawler *initCrawler = malloc(sizeof(struct huffmanCrawler));
    initCrawler->address = prefixPathNew();
    initCrawler->tree = tree;
    initCrawler->next = NULL;
    trav->head = initCrawler;
    trav->tail = initCrawler;
    return trav;
}

// traverse the tree via traverser until we find a leaf node,
// then return the character and it's encoding via a struct.
static struct encoder *huffmanTraverserPerform(struct huffmanTraverser *trav) {
    while (true) {
        struct huffmanCrawler *poppedCrawler = huffmanTraverserPop(trav);
        if (poppedCrawler->tree->character != NULL) {
            prefixPathRecord(poppedCrawler->address, ENCODING_END,
                             poppedCrawler->tree->character);
            struct encoder *charEnc =
                prefixPathEncoding(poppedCrawler->address);
            prefixPathFree(poppedCrawler->address);
            free(poppedCrawler);
            return charEnc;
        } else {
            if (poppedCrawler->tree->left != NULL) {
                struct huffmanCrawler *left = crawlLeft(poppedCrawler);
                huffmanTraverserAdd(trav, left);
            }
            if (poppedCrawler->tree->right != NULL) {
                struct huffmanCrawler *right = crawlRight(poppedCrawler);
                huffmanTraverserAdd(trav, right);
            }
            prefixPathFree(poppedCrawler->address);
            free(poppedCrawler);
        }
    }
}

// remove first element within the traverser
static struct huffmanCrawler *
huffmanTraverserPop(struct huffmanTraverser *trav) {
    struct huffmanCrawler *pop = trav->head;
    trav->head = trav->head->next;
    return pop;
}

// append an element to the huffman traverser
static void huffmanTraverserAdd(struct huffmanTraverser *trav,
                                struct huffmanCrawler *crawl) {
    if (trav->head == NULL) {
        trav->head = crawl;
        trav->tail = crawl;
    } else {
        trav->tail->next = crawl;
        trav->tail = trav->tail->next;
    }
}

// free huffman traverser
// asserts there are no more crawlers tracked
static void huffmanTraverserDeinit(struct huffmanTraverser *trav) {
    assert(trav->head == NULL);
    free(trav);
}

// implementation of huffmanCrawler functions

// move crawler one branch down to the left
static struct huffmanCrawler *crawlLeft(struct huffmanCrawler *crawl) {
    struct huffmanCrawler *newCrawler = crawlerCopy(crawl);
    newCrawler->tree = newCrawler->tree->left;
    prefixPathRecord(newCrawler->address, ENCODING_0, NULL);
    return newCrawler;
}

// move crawler one branch down to the right
static struct huffmanCrawler *crawlRight(struct huffmanCrawler *crawl) {
    struct huffmanCrawler *newCrawler = crawlerCopy(crawl);
    newCrawler->tree = newCrawler->tree->right;
    prefixPathRecord(newCrawler->address, ENCODING_1, NULL);
    return newCrawler;
}

// copy crawler instance
static struct huffmanCrawler *crawlerCopy(struct huffmanCrawler *crawl) {
    struct huffmanCrawler *copy = malloc(sizeof(struct huffmanCrawler));
    copy->next = NULL;
    // ERROR: This instruction causes all crawlers to operate on the
    // same path.
    // A dedicated copy function for prefix paths must be used.
    copy->address = prefixPathCopy(crawl->address);
    copy->tree = crawl->tree;
    return copy;
}

// implementation of prefixPath functions

// create an empty prefixPath structure
static struct prefixPath *prefixPathNew() {
    struct prefixPath *path = malloc(sizeof(struct prefixPath));
    path->head = NULL;
    path->length = 0;
    path->tail = NULL;
    return path;
}

// free entire prefixPath
static void prefixPathFree(struct prefixPath *path) {
    PrefixNode *currentNode = path->head;
    while (currentNode != NULL) {
        PrefixNode *trash = currentNode;
        currentNode = currentNode->next;
        if (trash->finalChar != NULL) {
            free(trash->finalChar);
        }
        free(trash);
    }
    free(path);
}

// generate an encoder for a specific character based on recorded
// path
static struct encoder *prefixPathEncoding(struct prefixPath *path) {
    assert(path->tail->finalChar != NULL);
    struct encoder *newEncoding = malloc(sizeof(struct encoder));
    newEncoding->charSum = characterSum(path->tail->finalChar);
    newEncoding->encoding = malloc(path->length);
    unsigned int ix = 0;
    for (PrefixNode *n = path->head; n != NULL; n = n->next) {
        if (n->dir == ENCODING_END) {
            newEncoding->encoding[ix] = '\0';
        } else {
            newEncoding->encoding[ix] = n->dir;
        }
        ix++;
    }
    newEncoding->encodingLength = path->length;
    assert(newEncoding->encoding[path->length - 1] == '\0');
    return newEncoding;
}

// update the path when traversing down a tree
static void prefixPathRecord(struct prefixPath *path, char dir,
                             char *finalChar) {
    // create the new node
    PrefixNode *newPathNode = malloc(sizeof(PrefixNode));
    newPathNode->dir = dir;
    newPathNode->next = NULL;
    if (finalChar != NULL) {
        newPathNode->finalChar = malloc(MAX_CHARACTER_LEN + 1);
        strncpy(newPathNode->finalChar, finalChar, MAX_CHARACTER_LEN + 1);
    } else {
        newPathNode->finalChar = NULL;
    }

    // add node to linked list.
    if (path->head == NULL) {
        path->head = newPathNode;
        path->tail = newPathNode;
    } else {
        path->tail->next = newPathNode;
        path->tail = path->tail->next;
    }

    path->length++;
}

// create a copy of a prefix path
static struct prefixPath *prefixPathCopy(struct prefixPath *orig) {
    struct prefixPath *copy = prefixPathNew();
    PrefixNode *origPointer = orig->head;
    while (origPointer != NULL) {
        prefixPathRecord(copy, origPointer->dir, origPointer->finalChar);
        origPointer = origPointer->next;
    }
    return copy;
}

// implementation of buffer functions

// create a new buffer with an initial size.
struct buffer *bufferInit(size_t size) {
    struct buffer *nBuf = malloc(sizeof(struct buffer));
    nBuf->charCount = 0;
    nBuf->capacity = size;
    nBuf->str = malloc(nBuf->capacity);
    // needed for string functions to work.
    // nBuf->str[0] = '\0';
    return nBuf;
}

// insert to buffer, resizing the string allocation if necessary
static void bufferInsert(struct buffer *buf, char *chars, unsigned int len) {
    unsigned int newCount = buf->charCount + len;
    if (newCount >= buf->capacity) {
        buf->capacity *= buf->capacity;
        char *resize = realloc(buf->str, buf->capacity);
        assert(resize != NULL);
        buf->str = resize;
    }
    // using strncat is very slow, so we do the ff instead
    for (unsigned int ix = 0; ix < len; ix++) {
        buf->str[buf->charCount + ix] = chars[ix];
    }
    buf->charCount = newCount;
}

// return the string stored in the buffer
static char *bufferGetStr(struct buffer *buf) {
    char *output = malloc(buf->charCount + 1);
    strncpy(output, buf->str, buf->charCount + 1);
    output[buf->charCount] = '\0';
    return output;
}

// free buffer
static void bufferFree(struct buffer *buf) {
    free(buf->str);
    free(buf);
}
