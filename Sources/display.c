/*
 * display.c
 *
 *  Created on: 24 Oct 2020
 *      Author: ladis
 */

#include "display.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <ctype.h>

uint8_t char_A[] = {
    0b11000001,
	0b10110111,
	0b10110111,
    0b10110111,
    0b11000001,
};

uint8_t char_H[] = {
	0b1000001,
	0b1110111,
	0b1000001,
};

uint8_t char_O[] = {
	0b1100011,
	0b1011101,
	0b1100011,
};

uint8_t char_J[] = {
  0b1001111,
  0b1011111,
  0b1000001,
};

static uint8_t alphabet[26][5] = {
	   {0b11000001,
		0b10110111,
		0b10110111,
	    0b10110111,
	    0b11000001}
};

void display_init(display_t *display)
{
	display->text = NULL;
	display->char_index = 0;
	display->col_index = 0;
	memset(display->matrix, 0xFF, 16);
}

void strtoupper(char *str)
{
	unsigned str_length = strlen(str);

	for (unsigned i = 0; i < str_length; i++) {
		str[i] = toupper(str[i]);
	}
}

bool display_set_text(display_t *display, char *text)
{
	free(display->text);

	display->text = malloc(strlen(text) + 1);
	if (display->text == NULL) {
		return false;
	}

	strcpy(display->text, text);
	strtoupper(display->text);

	return true;
}


uint8_t string_next_col(display_t *display)
{
	if (display->char_index >= strlen(display->text)) {
		return 0xFF;
	}

	if (display->col_index == 5) {
		display->col_index = 0;
		display->char_index += 1;
		return 0xFF;
	}
	else {
		char current_char = display->text[display->char_index];
		int alphabet_index = current_char - 'A';
		if (alphabet_index < 0 || alphabet_index >= 26) { // Character is not from range A-Z
			display->char_index += 1;
			return string_next_col(display);
		}
		uint8_t next_col = alphabet[alphabet_index][display->col_index];
		display->col_index += 1;
		return next_col;
	}
}

void display_push_col(display_t *display, uint8_t col)
{
	unsigned i;
	for (i = 1; i < 16; i++) {
		display->matrix[i-1] = display->matrix[i];
	}
	display->matrix[i-1] = col;
}

void display_shift_text_left(display_t *display)
{
	uint8_t next_col = string_next_col(display);
	display_push_col(display, next_col);
}

void display_free(display_t *display)
{
	free(display->text);
	display->text = NULL;
}



