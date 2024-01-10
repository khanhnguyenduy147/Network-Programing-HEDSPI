#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "../inc/session_manager.h"

/**
 * checkAccount - Check the validity of the user account.
 *
 * This function checks if a given username exists in the provided file and whether the account is valid or banned.
 *
 * @param username          The username to check.
 * @param file              The file containing account information.
 * @return                  1: Valid account, 0: Account doesn't exist, 2: Account is banned.
 */
int checkAccount(const char* username, const FILE *file) {
    char *line = NULL; 
    size_t len = 0;
    int found = 0; // The variable checks whether the login name exists in the file or not

    fseek(file, 0, SEEK_SET);

    while (getline(&line, &len, file) != -1) {
        char *storedUsername = (char*)malloc(strlen(line) + 1);
        
        int status;
        if (sscanf(line, "%s %d", storedUsername, &status) == 2) {
            if (strcmp(username, storedUsername) == 0 && status == 1) {
                found = 1;
                break; // Found valid account, exit the loop
            }else if(strcmp(username, storedUsername) == 0 && status == 0){
                found = 2; //Account is banned
                break;   
            }
        }

        free(storedUsername);
    }
    free(line);
    return found;   //return 1: Valid account, return 0: The account is not exist, return 2: The account is banned
}

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
int loginAccount(int *login, const char *buffer_request, int buffer_request_size, char *buffer_result, int buffer_result_size, const FILE *file){
    char *newline_position = strchr(buffer_request, '\n');
    if (newline_position != NULL) {
        *newline_position = '\0'; // Replace the newline character with a null-terminated character
    }
    char *username = malloc(buffer_request_size); 
    sscanf(buffer_request, "USER %[^\n]", username);
    int check = checkAccount(username, file);   
    if(*login == 1){
        memset(buffer_result, 0, buffer_result_size);
        strcpy(buffer_result, "214");   //Error code logged previously
    }else if(check == 1){
            memset(buffer_result, 0, buffer_result_size);
            strcpy(buffer_result, "110");
            return 1;
    }else if(check == 2){
        memset(buffer_result, 0, buffer_result_size);
        strcpy(buffer_result, "211");
    }else{
        memset(buffer_result, 0, buffer_result_size);
        strcpy(buffer_result, "212");
    }
    free(username);
    return 0;
}

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
int postMessage(int login, char *buffer_request, int buffer_request_size, char *buffer_result, int buffer_result_size){
    char *newline_position = strchr(buffer_request, '\n'); 
    if (newline_position != NULL) {
        *newline_position = '\0'; // Replace the newline character with a null-terminated character
    }
    char *article = malloc(buffer_request_size); 
    sscanf(buffer_request, "POST %[^\n]", article);
    if(login == 1){
        memset(buffer_result, 0, buffer_result_size);
        strcpy(buffer_result, "120");
    }else{
        memset(buffer_result, 0, buffer_result_size);
        strcpy(buffer_result, "221");
    }
    free(article);
}

/**
 * logoutAccount - Process the logout request.
 *
 * This function handles the logout request and updates the login status.
 *
 * @param login             Pointer to the login status (0: Not logged in, 1: Logged in).
 * @param buffer_result      The buffer to store the result of the logout request.
 * @param buffer_result_size Size of the result buffer.
 */

int logoutAccount(int *login, char *buffer_result, int buffer_result_size){
    if(*login == 1){
        memset(buffer_result, 0, buffer_result_size);
        strcpy(buffer_result, "130");
        *login = 0;
        return 1;
    }else{
        memset(buffer_result, 0, buffer_result_size);
        strcpy(buffer_result, "221");
        return 0;
    }
}