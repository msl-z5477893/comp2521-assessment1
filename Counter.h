// Interface to a Counter ADT that keeps count of distinct characters

// !!! DO NOT MODIFY THIS FILE !!!

#ifndef COUNTER_H
#define COUNTER_H

#include "character.h"

typedef struct counter *Counter;

// Used by CounterItems
struct item {
	char character[MAX_CHARACTER_LEN + 1];
	int freq;
};

/**
 * Returns a new empty counter
 */
Counter CounterNew(void);

/**
 * Frees all memory allocated to the counter
 */
void CounterFree(Counter c);

/**
 * Adds an occurrence of the given character to the counter
 */
void CounterAdd(Counter c, char *character);

/**
 * Returns the number of distinct characters added to the counter
 */
int CounterNumItems(Counter c);

/**
 * Returns the frequency of the given character
 */
int CounterGet(Counter c, char *character);

/**
 * Returns a dynamically allocated array containing a copy of each
 * distinct character in the counter and its frequency (in any order),
 * and sets *numItems to the number of distinct characters.
 */
struct item *CounterItems(Counter c, int *numItems);

#endif
