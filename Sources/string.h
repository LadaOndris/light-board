/*
 * string.h
 *
 *  Created on: 9 Nov 2020
 *      Author: Ladislav Ondris
 *      Email: xondri07@vutbr.cz
 */

#ifndef SOURCES_STRING_H_
#define SOURCES_STRING_H_

#include <stdlib.h>
#include <stdbool.h>

#define DEFAULT_STRING_CAPACITY 32

typedef struct string{
    char *string;
    size_t length;
    size_t capacity;
} string_t;

/**
 * Allocates and initializes the string_t.
 *
 * @return bool Returns true on success, otherwise false.
 */
bool string_init(string_t *string);

/**
 * Adds char to the end of string_t
 *
 * @param String to add the char to.
 * @param The character to be added.
 * @return bool Returns true on success, otherwise false.
 */
bool string_add_char(string_t *str, char c);

/**
 * Frees the string.
 *
 * @param String to be freed.
 */
void string_free(string_t *str);

#endif /* SOURCES_STRING_H_ */
