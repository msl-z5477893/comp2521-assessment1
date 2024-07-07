// Implementation of the Counter ADT
//
// THIS IS FOR TASK 2
//
// The Counter ADT is used to store and manage the count
// of various symbols in a file.
// Include Counter.h to use this structure.
//
// Completed by Michael Stephen Lape (z5477893)

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Counter.h"

// counter struct, left untouched
struct counter {
    struct counter *left;
    struct counter *right;
    char character[5];
    unsigned int count;
};

// CUSTOM STRUCTS AND FUNCTIONS

// limited Queue implementation, used for numList.
struct queue {
    struct node *head;
    struct node *tail;
};

// node struct for queue
struct node {
    void *content;
    struct node *next;
};

// queue functions
static struct queue *queueNew(void);
static struct node *queuePop(struct queue *);
static bool queueIsEmpty(struct queue *);
static void queueInsert(struct queue *, void *);
static void queueFree(struct queue *);

// create new counter tree
// performance: O(1)
Counter CounterNew(void) {
    struct counter *newCounter = malloc(sizeof(struct counter));
    newCounter->left = NULL;
    newCounter->right = NULL;
    newCounter->character[0] = '\0';
    newCounter->count = 0;
    return newCounter;
}

// free counter tree
// performance: O(n),
void CounterFree(Counter c) {
    // free branches first then itself
    if (c->left != NULL) {
        CounterFree(c->left);
    }
    if (c->right != NULL) {
        CounterFree(c->right);
    }
    free(c);
}

// record character to counter tree
// performance: O(h)
void CounterAdd(Counter c, char *character) {
    if (c->character[0] == '\0') {
        // case 0: initial tree is empty
        strncpy(c->character, character, 5);
        c->count++;
    } else if (!strcmp(c->character, character)) {
        c->count++;
    } else if (strcmp(c->character, character) < 0) {
        if (c->left == NULL) {
            c->left = CounterNew();
        }
        CounterAdd(c->left, character);

    } else if (strcmp(c->character, character) > 0) {
        if (c->right == NULL) {
            c->right = CounterNew();
        }
        CounterAdd(c->right, character);
    }
}

// count the number of unique items recorded by tree
// performance: O(n)
int CounterNumItems(Counter c) {
    // base case
    if (c == NULL) {
        return 0;
    }
    return 1 + CounterNumItems(c->left) + CounterNumItems(c->right);
}

// get the frequency of a given character.
// performance: O(h)
int CounterGet(Counter c, char *character) {
    Counter cPtr = c;
    while (cPtr != NULL) {
        if (!strcmp(cPtr->character, character)) {
            return cPtr->count;
        } else if (strcmp(cPtr->character, character) < 0) {
            cPtr = cPtr->left;
        } else if (strcmp(cPtr->character, character) > 0) {
            cPtr = cPtr->right;
        } else {
            // this is unreachable.
            // so return a nonsensical value if we get here somehow.
            return -1;
        }
    }
    return 0;
}

// creates an item list from the counter tree
// traverses tree in level order.
// performance: O(n)
struct item *CounterItems(Counter c, int *numItems) {
    struct item *items = malloc(sizeof(struct item) * CounterNumItems(c));
    int itemsArrayCount = 0;
    struct queue *counterQueue = queueNew();
    queueInsert(counterQueue, c);
    while (!queueIsEmpty(counterQueue)) {
        struct node *dequeued = queuePop(counterQueue);
        Counter counterToItem = dequeued->content;

        strncpy(items[itemsArrayCount].character, counterToItem->character, 5);
        items[itemsArrayCount].freq = counterToItem->count;

        itemsArrayCount++;
        if (counterToItem->left != NULL) {
            queueInsert(counterQueue, counterToItem->left);
        }
        if (counterToItem->right != NULL) {
            queueInsert(counterQueue, counterToItem->right);
        }
        free(dequeued);
    }
    queueFree(counterQueue);
    *numItems = itemsArrayCount;
    return items;
}

// helper functions for function items, uses internal queue implementation.

// create new queue.
// performance: O(1)
static struct queue *queueNew() {
    struct queue *q = malloc(sizeof(struct queue));
    q->head = NULL;
    q->tail = NULL;
    return q;
}

// insert item in queue.
// performance: O(1)
static void queueInsert(struct queue *q, void *item) {
    struct node *qNode = malloc(sizeof(struct node));
    qNode->content = item;
    qNode->next = NULL;
    // if head is NULL, then tail is also NULL
    // therefore the list is empty.
    if (q->head == NULL) {
        q->head = qNode;
        q->tail = qNode;
    } else {
        q->tail->next = qNode;
        q->tail = q->tail->next;
    }
}

// remove the first item in queue.
// note that popped item must be freed manually.
// performance: O(1)
static struct node *queuePop(struct queue *q) {
    struct node *popped = q->head;
    q->head = q->head->next;
    return popped;
}

// check if queue is empty.
// performance: O(1)
static bool queueIsEmpty(struct queue *q) { return q->head == NULL; }

// free queue
// queue must be empty, otherwise crash
// performance: O(1)
static void queueFree(struct queue *q) {
    assert(q->head == NULL);
    free(q);
}
