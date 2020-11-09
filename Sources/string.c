/*
 * string.c
 *
 *  Created on: 9 Nov 2020
 *      Author: Ladislav Ondris
 *      Email: xondri07@vutbr.cz
 */


#include "string.h"

bool string_init(string_t *string){
	string->length = 0;

	string->string = calloc(DEFAULT_STRING_CAPACITY, sizeof(char));
    if (string->string == NULL) {
    	string->capacity = 0;
    	return false;
    }
    else {
    	string->capacity = DEFAULT_STRING_CAPACITY * sizeof(char);
    	return true;
    }
}

bool string_realloc(string_t *str, size_t bytes_to_add){
    void *ptr = NULL;
    int new_cap = str->capacity;

    if(bytes_to_add < str->capacity){
        ptr = realloc(str->string, str->capacity * 2);
        new_cap *= 2;
    }
    else {
        ptr = realloc(str->string, str->capacity + bytes_to_add);
        new_cap += bytes_to_add;
    }

    if (!ptr) {
    	return false;
    }

    str->string = ptr;
    str->capacity = new_cap;
    return true;
}

bool string_add_char(string_t *str, char c){
    if(str->length + 1 == str->capacity){
        bool result = string_realloc(str, 1);
        if (!result) {
        	return false;
        }
    }

    str->string[str->length++] = c;
    str->string[str->length] = 0;
    return true;
}

void string_free(string_t *str){
    free(str->string);
    str->string = NULL;
    str->capacity = 0;
    str->length = 0;
}



