#ifndef LINKLISTDATASTREAM_H
#define LINKLISTDATASTREAM_H

// Data structure for an element in the linked list
struct Node {
    int login;               // Login status (0: Not logged in, 1: Logged in)
    char *text_data;         // Text data associated with the node
    int text_length;         // Length of the text data
    int socket_fd;           // Socket file descriptor
    struct Node *next;       // Pointer to the next node in the linked list
};

// Definition of the linked list structure
struct LinkedList {
    struct Node *head;       // Pointer to the head of the linked list
};

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
struct Node *createNode(int login, const char *text_data, int text_length, int socket_fd);

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
void addNode(struct LinkedList *list, int login, const char *text_data, int text_length, int socket_fd);

/**
 * removeNode - Remove an element from the linked list based on socket_fd.
 *
 * This function removes the node with the specified socket_fd from the linked list.
 * It uses a mutex to ensure thread safety when modifying the linked list.
 *
 * @param list          The linked list from which the node will be removed.
 * @param socket_fd     The socket file descriptor used to identify the node to be removed.
 */
void removeNode(struct LinkedList *list, int socket_fd);

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
int searchNode(struct LinkedList *list, int socket_fd);

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
struct Node *findNode(struct LinkedList *list, int socket_fd);

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
void appendText(struct LinkedList *list, int socket_fd, const char *additional_text);

/**
 * printList - Print the linked list elements.
 *
 * This function prints the elements of the linked list, including login status, text data, text length, and socket file descriptor.
 * It uses a mutex to ensure thread safety when accessing the linked list.
 *
 * @param list      The linked list to print.
 */
void printList(struct LinkedList *list);

/**
 * freeList - Free the memory of the linked list.
 *
 * This function frees the memory allocated for the elements of the linked list.
 * It uses a mutex to ensure thread safety when modifying the linked list.
 * After freeing the memory, it notifies about the data change.
 *
 * @param list      The linked list to free.
 */
void freeList(struct LinkedList *list);

#endif
