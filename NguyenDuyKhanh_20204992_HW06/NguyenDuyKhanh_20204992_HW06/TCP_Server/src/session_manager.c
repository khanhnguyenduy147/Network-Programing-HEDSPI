#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include "../inc/session_manager.h"

// Define a mutex for thread safety when accessing sessionList
static pthread_mutex_t sessionListMutex = PTHREAD_MUTEX_INITIALIZER;

/**
 * addSession - Add a new session to the session list.
 *
 * This function creates a new session, assigns values to its fields, and adds it to the beginning of the session list.
 *
 * @param sessionList       Pointer to the session list.
 * @param socketId          The socket ID of the session.
 * @param clientAddress     The client's IP address.
 * @param username          The username associated with the session.
 */
void addSession(struct Session** sessionList, int socketId, const char* clientAddress, const char* username) {
    pthread_mutex_lock(&sessionListMutex);

    struct Session* newSession = (struct Session*)malloc(sizeof(struct Session));

    // Assign values to the fields in the structure
    newSession->socketId = socketId;
    strncpy(newSession->clientAddress, clientAddress, sizeof(newSession->clientAddress));

    // Dynamically allocate memory for the username and copy the value into it
    newSession->username = strdup(username);

    newSession->isLoggedIn = 1; // When created, consider it as logged in
    newSession->next = *sessionList;

    // Update the head pointer of the session list
    *sessionList = newSession;

    pthread_mutex_unlock(&sessionListMutex);
}

/**
 * isUsernameLoggedIn - Check if an account is logged in from another client.
 *
 * This function traverses the session list to check if the specified username is logged in from another client.
 *
 * @param sessionList       Pointer to the session list.
 * @param username          The username to check.
 * @return                  1 if the account is logged in from another client, 0 otherwise.
 */
int isUsernameLoggedIn(const struct Session* sessionList, const char* username) {
    pthread_mutex_lock(&sessionListMutex);

    const struct Session* currentSession = sessionList;

    while (currentSession != NULL) {
        if (strcmp(currentSession->username, username) == 0 && currentSession->isLoggedIn) {
            // Found the account logged in from another client
            pthread_mutex_unlock(&sessionListMutex);
            return 1;
        }
        currentSession = currentSession->next;
    }

    // Account is not logged in from any other client
    pthread_mutex_unlock(&sessionListMutex);
    return 0;
}

/**
 * removeSession - Remove a session from the session list when the client logs out.
 *
 * This function removes a session from the session list when the associated client logs out.
 *
 * @param sessionList       Pointer to the session list.
 * @param socketId          The socket ID of the session to be removed.
 */
void removeSession(struct Session** sessionList, int socketId) {
    pthread_mutex_lock(&sessionListMutex);

    struct Session* currentSession = *sessionList;
    struct Session* prevSession = NULL;

    while (currentSession != NULL) {
        if (currentSession->socketId == socketId) {
            // Found the session to be removed
            if (prevSession != NULL) {
                // Session is not the head of the list
                prevSession->next = currentSession->next;
            } else {
                // Session is the head of the list
                *sessionList = currentSession->next;
            }

            // Free the memory of the username and then free the memory of the removed session
            free(currentSession->username);
            free(currentSession);
            break;
        }

        prevSession = currentSession;
        currentSession = currentSession->next;
    }

    pthread_mutex_unlock(&sessionListMutex);
}

/**
 * printSessionList - Print the list of sessions currently logged in (for debugging purposes).
 *
 * This function prints the details of each session in the session list.
 *
 * @param sessionList       Pointer to the session list.
 */
void printSessionList(const struct Session* sessionList) {
    pthread_mutex_lock(&sessionListMutex);

    const struct Session* currentSession = sessionList;

    while (currentSession != NULL) {
        printf("SocketID: %d, ClientAddress: %s, Username: %s\n",
               currentSession->socketId, currentSession->clientAddress, currentSession->username);
        currentSession = currentSession->next;
    }

    pthread_mutex_unlock(&sessionListMutex);
}
