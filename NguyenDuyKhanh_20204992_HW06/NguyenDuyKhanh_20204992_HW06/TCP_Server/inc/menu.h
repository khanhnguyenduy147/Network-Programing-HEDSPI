#ifndef MENU_H
#define MENU_H

/**
 * checkAccount - Check the validity of a user account in the given file.
 *
 * This function checks if a given username exists in the provided file and whether the account is valid or banned.
 *
 * @param username          The username to check.
 * @param file              The file containing account information.
 * @return                  1: Valid account, 0: Account doesn't exist, 2: Account is banned.
 */
int checkAccount(const char *username, const FILE *file);

/**
 * loginAccount - Process the login request.
 *
 * This function handles the login request, including parsing the provided username and checking its validity.
 *
 * @param login             Pointer to the login status (0: Not logged in, 1: Logged in).
 * @param buffer_request    The login request buffer.
 * @param buffer_request_size Size of the login request buffer.
 * @param buffer_result      The buffer to store the result of the login request.
 * @param buffer_result_size Size of the result buffer.
 * @param file              The file containing account information.
 */
int loginAccount(int *login, const char *buffer_request, int buffer_request_size, char *buffer_result, int buffer_result_size, const FILE *file);

/**
 * postMessage - Process the post message request.
 *
 * This function handles the post message request, including parsing the message and checking the login status.
 *
 * @param login             The login status (0: Not logged in, 1: Logged in).
 * @param buffer_request    The post message request buffer.
 * @param buffer_request_size Size of the post message request buffer.
 * @param buffer_result      The buffer to store the result of the request.
 * @param buffer_result_size Size of the result buffer.
 */
int postMessage(int login, char *buffer_request, int buffer_request_size, char *buffer_result, int buffer_result_size);

/**
 * logoutAccount - Process the logout request.
 *
 * This function handles the logout request and updates the login status.
 *
 * @param login             Pointer to the login status (0: Not logged in, 1: Logged in).
 * @param buffer_result      The buffer to store the result of the logout request.
 * @param buffer_result_size Size of the result buffer.
 */
int logoutAccount(int *login, char *buffer_result, int buffer_result_size);

#endif
