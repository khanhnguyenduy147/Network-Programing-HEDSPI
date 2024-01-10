#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H

// Structure to store information for each session
struct Session {
    int socketId;               // Socket identifier on the server
    char clientAddress[256];    // Client's address (can use sockaddr_in or char[] depending on the application)
    char* username;             // Logged-in username
    int isLoggedIn;             // Login status (0: not logged in, 1: logged in)
    struct Session* next;       // Pointer to the next session in the linked list
};

/**
 * addSession - Add a new session to the session list.
 *
 * This function creates a new session, assigns values to its fields, and adds it to the beginning of the session list.
 *
 * @param sessionList       Pointer to the session list.
 * @param socketId          The socket ID of the session.
 * @param clientAddress     The client's address.
 * @param username          The username associated with the session.
 */
void addSession(struct Session** sessionList, int socketId, const char* clientAddress, const char* username);

/**
 * isUsernameLoggedIn - Check if an account is logged in from another client.
 *
 * This function traverses the session list to check if the specified username is logged in from another client.
 *
 * @param sessionList       Pointer to the session list.
 * @param username          The username to check.
 * @return                  1 if the account is logged in from another client, 0 otherwise.
 */
int isUsernameLoggedIn(const struct Session* sessionList, const char* username);

/**
 * removeSession - Remove a session from the session list when the client logs out.
 *
 * This function removes a session from the session list when the associated client logs out.
 *
 * @param sessionList       Pointer to the session list.
 * @param socketId          The socket ID of the session to be removed.
 */
void removeSession(struct Session** sessionList, int socketId);

/**
 * printSessionList - Print the list of sessions currently logged in (for debugging purposes).
 *
 * This function prints the details of each session in the session list.
 *
 * @param sessionList       Pointer to the session list.
 */
void printSessionList(const struct Session* sessionList);

#endif
