#include <stdio.h>
#include "../inc/linklist_data_stream.h"
#ifndef CLIENTHANDLETHREAD_H
#define CLIENTHANDLETHREAD_H
/**
 * clientHandle - Handle incoming client connections and messages.
 *
 * This function is responsible for managing client connections and processing incoming messages.
 * It uses the select() system call to efficiently handle multiple connections.
 * The function creates separate threads to handle each client connection.
 *
 * @param file              The file containing account information.
 * @param listen_fd         The listening socket descriptor.
 * @param session_client    The linked list to manage client sessions.
 */
void clientHandle(FILE *file, int listen_fd, struct LinkedList session_client);

#endif
