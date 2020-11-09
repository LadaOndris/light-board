/*
 * display.c
 *
 *  Created on: 24 Oct 2020
 *      Author: Ladislav Ondris
 *      Email: xondri07@vutbr.cz
 */

#include "display.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <ctype.h>

static uint8_t alphabet[96][5] = {
		{
		0b11111111,
		0b11111111,
		0b11111111,
		0b11111111,
		0b11111111,
		},
		{
		0b11111111,
		0b11111111,
		0b10000110,
		0b11111111,
		0b11111111,
		},
		{
		0b11111111,
		0b10001111,
		0b11111111,
		0b10001111,
		0b11111111,
		},
		{
		0b11101011,
		0b10000000,
		0b11101011,
		0b10000000,
		0b11101011,
		},
		{
		0b11101101,
		0b11010101,
		0b10000000,
		0b11010101,
		0b11011011,
		},
		{
		0b10011101,
		0b10011011,
		0b11110111,
		0b11101100,
		0b11011100,
		},
		{
		0b11001001,
		0b10110110,
		0b10101010,
		0b11011101,
		0b11111010,
		},
		{
		0b11111111,
		0b10101111,
		0b10011111,
		0b11111111,
		0b11111111,
		},
		{
		0b11111111,
		0b11100011,
		0b11011101,
		0b10111110,
		0b11111111,
		},
		{
		0b11111111,
		0b10111110,
		0b11011101,
		0b11100011,
		0b11111111,
		},
		{
		0b11101011,
		0b11110111,
		0b11000001,
		0b11110111,
		0b11101011,
		},
		{
		0b11110111,
		0b11110111,
		0b11000001,
		0b11110111,
		0b11110111,
		},
		{
		0b11111111,
		0b11111010,
		0b11111001,
		0b11111111,
		0b11111111,
		},
		{
		0b11110111,
		0b11110111,
		0b11110111,
		0b11110111,
		0b11110111,
		},
		{
		0b11111111,
		0b11111100,
		0b11111100,
		0b11111111,
		0b11111111,
		},
		{
		0b11111101,
		0b11111011,
		0b11110111,
		0b11101111,
		0b11011111,
		},
		{
		0b11000001,
		0b10111010,
		0b10110110,
		0b10101110,
		0b11000001,
		},
		{
		0b11111111,
		0b11011110,
		0b10000000,
		0b11111110,
		0b11111111,
		},
		{
		0b11011110,
		0b10111100,
		0b10111010,
		0b10110110,
		0b11001110,
		},
		{
		0b10111101,
		0b10111110,
		0b10101110,
		0b10010110,
		0b10111001,
		},
		{
		0b11110011,
		0b11101011,
		0b11011011,
		0b10000000,
		0b11111011,
		},
		{
		0b10001101,
		0b10101110,
		0b10101110,
		0b10101110,
		0b10110001,
		},
		{
		0b11100001,
		0b11010110,
		0b10110110,
		0b10110110,
		0b11111001,
		},
		{
		0b10111111,
		0b10111000,
		0b10110111,
		0b10101111,
		0b10011111,
		},
		{
		0b11001001,
		0b10110110,
		0b10110110,
		0b10110110,
		0b11001001,
		},
		{
		0b11001111,
		0b10110110,
		0b10110110,
		0b10110101,
		0b11000011,
		},
		{
		0b11111111,
		0b11001001,
		0b11001001,
		0b11111111,
		0b11111111,
		},
		{
		0b11111111,
		0b11001010,
		0b11001001,
		0b11111111,
		0b11111111,
		},
		{
		0b11110111,
		0b11101011,
		0b11011101,
		0b10111110,
		0b11111111,
		},
		{
		0b11101011,
		0b11101011,
		0b11101011,
		0b11101011,
		0b11101011,
		},
		{
		0b11111111,
		0b10111110,
		0b11011101,
		0b11101011,
		0b11110111,
		},
		{
		0b11011111,
		0b10111111,
		0b10111010,
		0b10110111,
		0b11001111,
		},
		{
		0b11011001,
		0b10110110,
		0b10110000,
		0b10111110,
		0b11000001,
		},
		{
		0b11000000,
		0b10111011,
		0b10111011,
		0b10111011,
		0b11000000,
		},
		{
		0b10000000,
		0b10110110,
		0b10110110,
		0b10110110,
		0b11001001,
		},
		{
		0b11000001,
		0b10111110,
		0b10111110,
		0b10111110,
		0b11011101,
		},
		{
		0b10000000,
		0b10111110,
		0b10111110,
		0b11011101,
		0b11100011,
		},
		{
		0b10000000,
		0b10110110,
		0b10110110,
		0b10110110,
		0b10111110,
		},
		{
		0b10000000,
		0b10110111,
		0b10110111,
		0b10110111,
		0b10111111,
		},
		{
		0b11000001,
		0b10111110,
		0b10110110,
		0b10110110,
		0b11010000,
		},
		{
		0b10000000,
		0b11110111,
		0b11110111,
		0b11110111,
		0b10000000,
		},
		{
		0b11111111,
		0b10111110,
		0b10000000,
		0b10111110,
		0b11111111,
		},
		{
		0b11111101,
		0b11111110,
		0b10111110,
		0b10000001,
		0b10111111,
		},
		{
		0b10000000,
		0b11110111,
		0b11101011,
		0b11011101,
		0b10111110,
		},
		{
		0b10000000,
		0b11111110,
		0b11111110,
		0b11111110,
		0b11111110,
		},
		{
		0b10000000,
		0b11011111,
		0b11100111,
		0b11011111,
		0b10000000,
		},
		{
		0b10000000,
		0b11101111,
		0b11110111,
		0b11111011,
		0b10000000,
		},
		{
		0b11000001,
		0b10111110,
		0b10111110,
		0b10111110,
		0b11000001,
		},
		{
		0b10000000,
		0b10110111,
		0b10110111,
		0b10110111,
		0b11001111,
		},
		{
		0b11000001,
		0b10111110,
		0b10111010,
		0b10111101,
		0b11000010,
		},
		{
		0b10000000,
		0b10110111,
		0b10110011,
		0b10110101,
		0b11001110,
		},
		{
		0b11001110,
		0b10110110,
		0b10110110,
		0b10110110,
		0b10111001,
		},
		{
		0b10111111,
		0b10111111,
		0b10000000,
		0b10111111,
		0b10111111,
		},
		{
		0b10000001,
		0b11111110,
		0b11111110,
		0b11111110,
		0b10000001,
		},
		{
		0b10000011,
		0b11111101,
		0b11111110,
		0b11111101,
		0b10000011,
		},
		{
		0b10000001,
		0b11111110,
		0b11110001,
		0b11111110,
		0b10000001,
		},
		{
		0b10011100,
		0b11101011,
		0b11110111,
		0b11101011,
		0b10011100,
		},
		{
		0b10001111,
		0b11110111,
		0b11111000,
		0b11110111,
		0b10001111,
		},
		{
		0b10111100,
		0b10111010,
		0b10110110,
		0b10101110,
		0b10011110,
		},
		{
		0b11111111,
		0b10000000,
		0b10111110,
		0b10111110,
		0b11111111,
		},
		{
		0b11011111,
		0b11101111,
		0b11110111,
		0b11111011,
		0b11111101,
		},
		{
		0b11111111,
		0b10111110,
		0b10111110,
		0b10000000,
		0b11111111,
		},
		{
		0b11101111,
		0b11011111,
		0b10111111,
		0b11011111,
		0b11101111,
		},
		{
		0b11111110,
		0b11111110,
		0b11111110,
		0b11111110,
		0b11111110,
		},
		{
		0b11111111,
		0b10111111,
		0b11011111,
		0b11101111,
		0b11111111,
		},
		{
		0b11111101,
		0b11101010,
		0b11101010,
		0b11101010,
		0b11110000,
		},
		{
		0b10000000,
		0b11110110,
		0b11110110,
		0b11110110,
		0b11111001,
		},
		{
		0b11110001,
		0b11101110,
		0b11101110,
		0b11101110,
		0b11101110,
		},
		{
		0b11111001,
		0b11110110,
		0b11110110,
		0b11110110,
		0b10000000,
		},
		{
		0b11110001,
		0b11101010,
		0b11101010,
		0b11101010,
		0b11110010,
		},
		{
		0b11111111,
		0b11110111,
		0b11000000,
		0b10110111,
		0b11011111,
		},
		{
		0b11110110,
		0b11101010,
		0b11101010,
		0b11101010,
		0b11100001,
		},
		{
		0b10000000,
		0b11110111,
		0b11110111,
		0b11110111,
		0b11111000,
		},
		{
		0b11111111,
		0b11111111,
		0b11010000,
		0b11111111,
		0b11111111,
		},
		{
		0b11111101,
		0b11111110,
		0b11111110,
		0b10100001,
		0b11111111,
		},
		{
		0b11111111,
		0b10000000,
		0b11111011,
		0b11110101,
		0b11101110,
		},
		{
		0b11111111,
		0b10111110,
		0b10000000,
		0b11111110,
		0b11111111,
		},
		{
		0b11100000,
		0b11101111,
		0b11110001,
		0b11101111,
		0b11100000,
		},
		{
		0b11100000,
		0b11110111,
		0b11101111,
		0b11101111,
		0b11110000,
		},
		{
		0b11110001,
		0b11101110,
		0b11101110,
		0b11101110,
		0b11110001,
		},
		{
		0b11100000,
		0b11101011,
		0b11101011,
		0b11101011,
		0b11110111,
		},
		{
		0b11110111,
		0b11101011,
		0b11101011,
		0b11101011,
		0b11100000,
		},
		{
		0b11100000,
		0b11110111,
		0b11101111,
		0b11101111,
		0b11110111,
		},
		{
		0b11110110,
		0b11101010,
		0b11101010,
		0b11101010,
		0b11101101,
		},
		{
		0b11101111,
		0b11101111,
		0b10000001,
		0b11101110,
		0b11101101,
		},
		{
		0b11100001,
		0b11111110,
		0b11111110,
		0b11111110,
		0b11100001,
		},
		{
		0b11100011,
		0b11111101,
		0b11111110,
		0b11111101,
		0b11100011,
		},
		{
		0b11100001,
		0b11111110,
		0b11111001,
		0b11111110,
		0b11100001,
		},
		{
		0b11101110,
		0b11110101,
		0b11111011,
		0b11110101,
		0b11101110,
		},
		{
		0b11101111,
		0b11110110,
		0b11111001,
		0b11110111,
		0b11101111,
		},
		{
		0b11101110,
		0b11101100,
		0b11101010,
		0b11100110,
		0b11101110,
		},
		{
		0b11110111,
		0b11001001,
		0b10111110,
		0b10111110,
		0b11111111,
		},
		{
		0b11111111,
		0b11111111,
		0b10000000,
		0b11111111,
		0b11111111,
		},
		{
		0b11111111,
		0b10111110,
		0b10111110,
		0b11001001,
		0b11110111,
		},
		{
		0b11101111,
		0b11011111,
		0b11011111,
		0b11011111,
		0b10111111,
		},
		{
		0b10000000,
		0b10111110,
		0b10111110,
		0b10111110,
		0b10000000,
		},
};

void display_init(display_t *display)
{
	display->text = NULL;
	display->char_index = 0;
	display->col_index = 0;
	memset(display->matrix, 0xFF, 16);
}

bool display_set_text(display_t *display, char *text)
{
	free(display->text);
	display_init(display);

	display->text = malloc(strlen(text) + 1);
	if (display->text == NULL) {
		return false;
	}

	strcpy(display->text, text);

	return true;
}


uint8_t string_next_col(display_t *display)
{
	unsigned len = strlen(display->text);
	if (display->char_index >= len) {
		if (display->char_index < len + 15) { // If there is still text on the display
			display->char_index += 1;
			return 0xFF;
		}
		else { // Start from the beginning
			display->char_index = 0;
			display->col_index = 0;
			return string_next_col(display);
		}
	}

	if (display->col_index == 5) {
		display->col_index = 0;
		display->char_index += 1;
		return 0xFF;
	}
	else {
		char current_char = display->text[display->char_index];
		int alphabet_index = current_char - 32;

		if (alphabet_index < 0) {
			uint8_t space_char = alphabet[0][display->col_index];
			display->char_index += 1;
			return space_char;
		}
		else if (alphabet_index > 127 - 32) {
			uint8_t del_char = alphabet[127 - 32][display->col_index];
			display->char_index += 1;
			return del_char;
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



