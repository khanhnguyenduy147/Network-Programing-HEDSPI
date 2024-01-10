#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "../inc/linklist_data_stream.h"

// Structure to manage synchronization for the linked list
struct {
    pthread_mutex_t mutex;       // Mutex for exclusive access to shared data
    pthread_cond_t cond;         // Condition variable to signal changes in the data
    int data_available;          // Variable indicating the availability of new data
} socket_data = {PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER, 0};

/**
 * createNode - Create a new node for the linked list.
 *
 * This function allocates memory for a new node and initializes its fields with the provided values.
 *
 * @param login         The login status for the node.
 * @param text_data     The text data associated with the node.
 * @param text_length   The length of the text data.
 * @param socket_fd     The socket file descriptor associated with the node.
 * @return              A pointer to the newly created node.
 */
struct Node *createNode(int login, const char *text_data, int text_length, int socket_fd) {
    struct Node *newNode = (struct Node *)malloc(sizeof(struct Node));
    if (!newNode) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    newNode->login = login;
    newNode->text_data = NULL;
    newNode->text_length = text_length;
    newNode->socket_fd = socket_fd;
    newNode->next = NULL;

    return newNode;
}

/**
 * addNode - Add an element to the linked list.
 *
 * This function creates a new node with the provided values and adds it to the end of the linked list.
 * It also uses a mutex to ensure thread safety when modifying the linked list.
 *
 * @param list          The linked list to which the node will be added.
 * @param login         The login status for the new node.
 * @param text_data     The text data associated with the new node.
 * @param text_length   The length of the text data.
 * @param socket_fd     The socket file descriptor associated with the new node.
 */
void addNode(struct LinkedList *list, int login, const char *text_data, int text_length, int socket_fd) {
    struct Node *newNode = createNode(login, text_data, text_length, socket_fd);

    pthread_mutex_lock(&socket_data.mutex);

    if (!list->head) {
        list->head = newNode;
    } else {
        struct Node *current = list->head;
        while (current->next) {
            current = current->next;
        }
        current->next = newNode;
    }

    // Notify about the data change
    socket_data.data_available = 1;
    pthread_cond_signal(&socket_data.cond);

    pthread_mutex_unlock(&socket_data.mutex);
}

/**
 * removeNode - Remove an element from the linked list based on socket_fd.
 *
 * This function removes the node with the specified socket_fd from the linked list.
 * It uses a mutex to ensure thread safety when modifying the linked list.
 *
 * @param list          The linked list from which the node will be removed.
 * @param socket_fd     The socket file descriptor used to identify the node to be removed.
 */
void removeNode(struct LinkedList *list, int socket_fd) {
    if (!list->head) {
        return;
    }

    pthread_mutex_lock(&socket_data.mutex);

    struct Node *current = list->head;
    struct Node *prev = NULL;

    while (current) {
        if (current->socket_fd == socket_fd) {
            if (prev) {
                prev->next = current->next;
            } else {
                list->head = current->next;
            }

            free(current->text_data);
            free(current);

            // Notify about the data change
            socket_data.data_available = 1;
            pthread_cond_signal(&socket_data.cond);

            pthread_mutex_unlock(&socket_data.mutex);

            return;
        }
        prev = current;
        current = current->next;
    }

    pthread_mutex_unlock(&socket_data.mutex);
}

/**
 * searchNode - Search for an element in the linked list based on socket_fd.
 *
 * This function checks if a node with the specified socket_fd exists in the linked list.
 * It uses a mutex to ensure thread safety when accessing the linked list.
 *
 * @param list          The linked list to search.
 * @param socket_fd     The socket file descriptor to search for in the linked list.
 * @return              1 if the element with the given socket_fd exists, 0 otherwise.
 */
int searchNode(struct LinkedList *list, int socket_fd) {
    pthread_mutex_lock(&socket_data.mutex);

    struct Node *current = list->head;

    while (current) {
        if (current->socket_fd == socket_fd) {
            pthread_mutex_unlock(&socket_data.mutex);
            return 1; // The element with this socket_fd exists in the list
        }
        current = current->next;
    }

    pthread_mutex_unlock(&socket_data.mutex);
    return 0; // The element with socket_fd is not found in the list
}

/**
 * findNode - Find and return a node based on socket_fd in the linked list.
 *
 * This function searches for a node with the specified socket_fd in the linked list and returns the node if found.
 * It uses a mutex to ensure thread safety when accessing the linked list.
 *
 * @param list          The linked list to search.
 * @param socket_fd     The socket file descriptor to search for in the linked list.
 * @return              A pointer to the found node, or NULL if the node with the given socket_fd is not found.
 */
struct Node *findNode(struct LinkedList *list, int socket_fd) {
    pthread_mutex_lock(&socket_data.mutex);

    struct Node *current = list->head;

    while (current) {
        if (current->socket_fd == socket_fd) {
            pthread_mutex_unlock(&socket_data.mutex);
            return current; // Return the node if found
        }
        current = current->next;
    }

    pthread_mutex_unlock(&socket_data.mutex);
    return NULL; // Return NULL if the node with socket_fd is not found
}

/**
 * appendText - Append text to the end of the data of the element with a specified socket_fd.
 *
 * This function adds additional_text to the text_data of the node with the specified socket_fd.
 * It uses a mutex to ensure thread safety when modifying the linked list.
 *
 * @param list              The linked list containing the target node.
 * @param socket_fd         The socket file descriptor of the node to which text will be appended.
 * @param additional_text   The text to append to the existing text_data.
 */
void appendText(struct LinkedList *list, int socket_fd, const char *additional_text) {
    pthread_mutex_lock(&socket_data.mutex);

    struct Node *current = list->head;

    while (current) {
        if (current->socket_fd == socket_fd) {
            int old_length = current->text_length;
            int additional_length = strlen(additional_text);

            // Resize the memory to accommodate the new text
            current->text_data = realloc(current->text_data, old_length + additional_length + 1);
            if (!current->text_data) {
                perror("Memory allocation failed");
                exit(EXIT_FAILURE);
            }

            // Append the new text to the end of the current data
            strcpy(current->text_data + old_length, additional_text);
            current->text_length += additional_length;

            // Notify about the data change
            socket_data.data_available = 1;
            pthread_cond_signal(&socket_data.cond);

            pthread_mutex_unlock(&socket_data.mutex);

            return;
        }
        current = current->next;
    }

    pthread_mutex_unlock(&socket_data.mutex);
}

/**
 * printList - Print the linked list elements.
 *
 * This function prints the elements of the linked list, including login status, text data, text length, and socket file descriptor.
 * It uses a mutex to ensure thread safety when accessing the linked list.
 *
 * @param list      The linked list to print.
 */
void printList(struct LinkedList *list) {
    pthread_mutex_lock(&socket_data.mutex);

    struct Node *current = list->head;

    while (current) {
        printf("Login: %d, Text Data: %s, Text Length: %d, Socket FD: %d\n",
               current->login, current->text_data, current->text_length, current->socket_fd);
        current = current->next;
    }

    pthread_mutex_unlock(&socket_data.mutex);
}

/**
 * freeList - Free the memory of the linked list.
 *
 * This function frees the memory allocated for the elements of the linked list.
 * It uses a mutex to ensure thread safety when modifying the linked list.
 * After freeing the memory, it notifies about the data change.
 *
 * @param list      The linked list to free.
 */
void freeList(struct LinkedList *list) {
    pthread_mutex_lock(&socket_data.mutex);

    struct Node *current = list->head;
    struct Node *next;

    while (current) {
        next = current->next;
        free(current->text_data);
        free(current);
        current = next;
    }

    list->head = NULL;

    // Notify about the data change
    socket_data.data_available = 1;
    pthread_cond_signal(&socket_data.cond);

    pthread_mutex_unlock(&socket_data.mutex);
}
