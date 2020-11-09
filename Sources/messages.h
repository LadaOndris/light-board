/*
 * messages.h
 *
 *  Created on: 25 Oct 2020
 *      Author: Ladislav Ondris
 *      Email: xondri07@vutbr.cz
 */

#ifndef SOURCES_MESSAGES_H_
#define SOURCES_MESSAGES_H_

/**
 * Messenger is an entity keeping messages to display
 * and provides functionality to switch between them.
 */
typedef struct messenger {
	char **messages;
	unsigned messages_count;
	int message_index;
} messenger_t;

/**
 * Initializes a messenger struct with given messages.
 *
 * @param messenger The messenger structure
 * @param messages List of messages. They are not copied.
 * @param messages_count Number of messages
 */
void messenger_init(messenger_t *messenger, char **messages, unsigned messages_count);

/**
 * Gets the next message in the list of messages.
 * The first message in the list is returned after the last one.
 */
char *messenger_get_next(messenger_t *messenger);

/**
 * Gets the previous message in the list of messages.
 * The last message in the list is returned after the first one.
 */
char *messenger_get_previous(messenger_t *messenger);


#endif /* SOURCES_MESSAGES_H_ */
