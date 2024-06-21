// Main program for testing the Counter ADT

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "character.h"
#include "Counter.h"

static void test1(void);
static void test2(void);
static void test3(void);

int main(void) {
    test1();
    test2();
    test3();
}

static void test1(void) {
    Counter counter = CounterNew();

    char *characters[10] = {
        "a", "d", "c", "a", "b", "d", "a", "a", "c", "d",
    };

    char character[MAX_CHARACTER_LEN + 1];
    for (int i = 0; i < 10; i++) {
        strcpy(character, characters[i]);
        CounterAdd(counter, character);
    }

    assert(CounterNumItems(counter) == 4);

    CounterFree(counter);

    printf("Test 1 passed!\n");
}

static void test2(void) {
    Counter counter = CounterNew();

    char *characters[10] = {
        "a", "d", "c", "a", "b", "d", "a", "a", "c", "d"
    };

    char character[MAX_CHARACTER_LEN + 1];
    for (int i = 0; i < 10; i++) {
        strcpy(character, characters[i]);
        CounterAdd(counter, character);
    }

    assert(CounterGet(counter, "a") == 4);
    assert(CounterGet(counter, "b") == 1);
    assert(CounterGet(counter, "c") == 2);
    assert(CounterGet(counter, "d") == 3);
    assert(CounterGet(counter, "e") == 0);

    CounterFree(counter);

    printf("Test 2 passed!\n");
}

static void test3(void) {
    Counter counter = CounterNew();

    char *characters[10] = {
        "a", "d", "c", "a", "b", "d", "a", "a", "c", "d"
    };

    char character[MAX_CHARACTER_LEN + 1];
    for (int i = 0; i < 10; i++) {
        strcpy(character, characters[i]);
        CounterAdd(counter, character);
    }

    int numItems = 0;
    struct item *items = CounterItems(counter, &numItems);
    assert(numItems == 4);

    // Simple sort to put items in alphabetical order
    for (int i = 0; i < numItems; i++) {
        int min = i;
        for (int j = i + 1; j < numItems; j++) {
            if (strcmp(items[j].character, items[min].character) < 0) {
                min = j;
            }
        }

        struct item tmp = items[i];
        items[i] = items[min];
        items[min] = tmp;
    }

    assert(strcmp(items[0].character, "a") == 0 && items[0].freq == 4);
    assert(strcmp(items[1].character, "b") == 0 && items[1].freq == 1);
    assert(strcmp(items[2].character, "c") == 0 && items[2].freq == 2);
    assert(strcmp(items[3].character, "d") == 0 && items[3].freq == 3);

    free(items);
    CounterFree(counter);

    printf("Test 3 passed!\n");
}
