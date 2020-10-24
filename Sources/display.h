/*
 * display.h
 *
 *  Created on: 24 Oct 2020
 *      Author: ladis
 */

#ifndef SOURCES_DISPLAY_H_
#define SOURCES_DISPLAY_H_

#include <stdint.h>
#include <stdbool.h>

typedef struct display {
	uint8_t matrix[16];
	char *text;
	int char_index;
	int col_index;
} display_t;

void display_init(display_t *display);
bool display_set_text(display_t *display, char *text);
void display_shift_text_left(display_t *display);
void display_free(display_t *display);


#endif /* SOURCES_DISPLAY_H_ */
