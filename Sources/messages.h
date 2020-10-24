/*
 * messages.h
 *
 *  Created on: 25 Oct 2020
 *      Author: ladis
 */

#ifndef SOURCES_MESSAGES_H_
#define SOURCES_MESSAGES_H_

typedef struct messenger {
	char **messages;
	unsigned messages_count;
	int message_index;
} messenger_t;

void messenger_init(messenger_t *messenger, char **messages, unsigned messages_count);
char *messenger_get_next(messenger_t *messenger);
char *messenger_get_previous(messenger_t *messenger);


#endif /* SOURCES_MESSAGES_H_ */
