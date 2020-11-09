/*
 * display.h
 *
 *  Created on: 24 Oct 2020
 *      Author: Ladislav Ondris
 *      Email: xondri07@vutbr.cz
 */

#ifndef SOURCES_DISPLAY_H_
#define SOURCES_DISPLAY_H_

#include <stdint.h>
#include <stdbool.h>

/**
 * Display is an entity keeping inner representation of a matrix display.
 * It allows changing displayed text, and shifting it.
 */
typedef struct display {
	uint8_t matrix[16];
	char *text;
	int char_index;
	int col_index;
} display_t;

/**
 * Initializes a display struct.
 * Initializes the display to show nothing.
 */
void display_init(display_t *display);

/**
 * Sets a new text to be shown on the display.
 *
 * @param text The text to be shown. It is copied.
 */
bool display_set_text(display_t *display, char *text);

/**
 * Shifts the text by one column to the left.
 * When all characters of the text have been shown,
 * it starts again from the beginning.
 */
void display_shift_text_left(display_t *display);

/**
 * Frees the display variables.
 */
void display_free(display_t *display);


#endif /* SOURCES_DISPLAY_H_ */
