/*
 * messages.c
 *
 *  Created on: 25 Oct 2020
 *      Author: Ladislav Ondris
 *      Email: xondri07@vutbr.cz
 */

#include <stdlib.h>
#include "messages.h"


void messenger_init(messenger_t *messenger, char **messages, unsigned messages_count)
{
	messenger->messages = messages;
	messenger->messages_count = messages_count;
	messenger->message_index = -1;
}

char *messenger_get_next(messenger_t *messenger)
{
	if (messenger == NULL || messenger->messages_count <= 0) {
		return NULL;
	}

	messenger->message_index++;
	if (messenger->message_index >= messenger->messages_count) {
		messenger->message_index = 0;
	}
	return messenger->messages[messenger->message_index];
}

char *messenger_get_previous(messenger_t *messenger)
{
	if (messenger == NULL || messenger->messages_count <= 0) {
		return NULL;
	}
	messenger->message_index--;
	if (messenger->message_index < 0) {
		messenger->message_index = messenger->messages_count - 1;
	}
	return messenger->messages[messenger->message_index];
}



