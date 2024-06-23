// Implementation of the Counter ADT
// THIS IS FOR TASK 2

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Counter.h"

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

struct node {
        void *content;
        struct node *next;
};

struct queue *queueNew(void);
void queueInsert(struct queue *, void *);
struct node *queuePop(struct queue *);
bool queueIsEmpty(struct queue *);
void queueFree(struct queue *);

Counter CounterNew(void) {
        struct counter *new_counter = malloc(sizeof(struct counter));
        new_counter->left = NULL;
        new_counter->right = NULL;
        new_counter->character[0] = '\0';
        new_counter->count = 0;
        return new_counter;
}

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

void CounterAdd(Counter c, char *character) {
        if (c->character[0] == '\0') {
                // case 0: initial tree is empty
                // printf("Empty tree node found! inserting %s...\n",
                // character);
                strncpy(c->character, character, 5);
                c->count++;
        } else if (!strcmp(c->character, character)) {
                // printf(
                //     "recording character \"%s\", which is tallied %d
                //     times.\n", c->character, c->count);
                c->count++;
        } else if (strcmp(c->character, character) < 0) {
                if (c->left == NULL) {
                        c->left = CounterNew();
                }
                // printf("Entering left of tree...\n");
                CounterAdd(c->left, character);

        } else if (strcmp(c->character, character) > 0) {
                if (c->right == NULL) {
                        c->right = CounterNew();
                }
                // printf("Entering right of tree...\n");
                CounterAdd(c->right, character);
        }
        // printf("Function call exited.\n");
}

int CounterNumItems(Counter c) {
        // base case
        if (c == NULL) {
                return 0;
        }
        return 1 + CounterNumItems(c->left) + CounterNumItems(c->right);
}

int CounterGet(Counter c, char *character) {
        Counter c_ptr = c;
        while (c_ptr != NULL) {
                if (!strcmp(c_ptr->character, character)) {
                        // printf("Character %s has been recorded with freq
                        // %d\n", c_ptr->character, c_ptr->count);
                        return c_ptr->count;
                } else if (strcmp(c_ptr->character, character) < 0) {
                        c_ptr = c_ptr->left;
                } else if (strcmp(c_ptr->character, character) > 0) {
                        c_ptr = c_ptr->right;
                } else {
                        return -1;
                }
        }
        return 0;
}

// creates an item list from the counter tree
// traverses tree in level order.
struct item *CounterItems(Counter c, int *numItems) {
        struct item *items = malloc(sizeof(struct item) * CounterNumItems(c));
        int items_arr_count = 0;
        struct queue *counterQueue = queueNew();
        queueInsert(counterQueue, c);
        while (!queueIsEmpty(counterQueue)) {
                // printf("CounterItems iteration %d.\n", items_arr_count);
                struct node *dequeued = queuePop(counterQueue);
                Counter counter_to_item = dequeued->content;

                strncpy(items[items_arr_count].character,
                        counter_to_item->character, 5);
                items[items_arr_count].freq = counter_to_item->count;

                items_arr_count++;
                if (counter_to_item->left != NULL) {
                        queueInsert(counterQueue, counter_to_item->left);
                }
                if (counter_to_item->right != NULL) {
                        queueInsert(counterQueue, counter_to_item->right);
                }
                free(dequeued);
        }
        queueFree(counterQueue);
        *numItems = items_arr_count;
        return items;
}

// helper functions, uses Queue implementation above.

// create new queue.
struct queue *queueNew() {
        struct queue *q = malloc(sizeof(struct queue));
        q->head = NULL;
        q->tail = NULL;
        return q;
}

// insert item in queue.
void queueInsert(struct queue *q, void *item) {
        struct node *q_node = malloc(sizeof(struct node));
        q_node->content = item;
        q_node->next = NULL;
        // if head is NULL, then tail is also NULL
        // therefore the list is empty.
        if (q->head == NULL) {
                q->head = q_node;
                q->tail = q_node;
        } else {
                q->tail->next = q_node;
                q->tail = q->tail->next;
        }
}

// remove the first item in queue.
// note that popped item must be freed manually.
struct node *queuePop(struct queue *q) {
        struct node *popped = q->head;
        q->head = q->head->next;
        return popped;
}

// check if queue is empty.
bool queueIsEmpty(struct queue *q) { return q->head == NULL; }

// free queue
void queueFree(struct queue *q) {
        assert(q->head == NULL);
        free(q);
}
