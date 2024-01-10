#include <sys/select.h>
#include "../inc/linklist_data_stream.h"

#ifndef CLIENTHANDLETHREAD_H
#define CLIENTHANDLETHREAD_H

struct ThreadParams {
    fd_set allset;
    FILE *file;
    int *client_array;
    int listen_fd;
    int maxfd;
    int maxi;
    struct LinkedList *session_client;
};
/**
 * clientHandleThread - Handle communication for an individual client in a separate thread.
 *
 * This function is responsible for managing the communication with an individual client in a separate thread.
 * It processes incoming messages and updates the client's session information.
 *
 * @param params    Pointer to a struct containing parameters for the thread.
 */
void* clientHandleThread(void* params);

#endif
