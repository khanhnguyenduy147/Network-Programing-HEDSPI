#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include "../inc/client_handle_thread.h"
#include "../inc/limit_client.h"
#include "../inc/menu.h"  
#include "../inc/linklist_data_stream.h"  

/**
 * clientHandleThread - Handle communication for an individual client in a separate thread.
 *
 * This function is responsible for managing the communication with an individual client in a separate thread.
 * It processes incoming messages and updates the client's session information.
 *
 * @param params    Pointer to a struct containing parameters for the thread.
 */
void* clientHandleThread(void* params) {
    printf("Thread start!\n");
    struct ThreadParams* threadParams = (struct ThreadParams*)params;
    fd_set allset = threadParams->allset;
    FILE *file = threadParams->file;
    int* client_array = threadParams->client_array;
    int listen_fd = threadParams->listen_fd;
    int maxfd = threadParams->maxfd;
    int maxi = threadParams->maxi;
    struct LinkedList *session_client = threadParams->session_client;

    char buffer[BUFF_SIZE];
    int sockfd;
    fd_set readfds;
    int nready;
    int total_client = 0;

    for (int i = 0; i <= maxi; i++) {
        printf("[%d] \n", client_array[i]);
            if (client_array[i] > 0){
                total_client++;
            }
    }
    FD_CLR(listen_fd, &allset);

    while (total_client) {
        readfds = allset;
        nready = select(maxfd + 1, &readfds, NULL, NULL, NULL);
        if (nready < 0) {
            perror("\nError in select");
            return 0;
        }
    
        for (int i = 0; i <= maxi; i++) {
            if ((sockfd = client_array[i]) < 0)
                continue;

            if (FD_ISSET(sockfd, &readfds)) {
                int login = 0;
                int bytes_received;
                char *partial_data = NULL;
                int partial_data_length = 0;
                struct Node *client_node;
                
                // If the client is not in the session list, add it
                if(!searchNode(session_client, sockfd)){
                    addNode(session_client, login, partial_data, partial_data_length, sockfd);
                }

                client_node = findNode(session_client, sockfd);

                // Set socket to non-blocking mode
                    int flags = fcntl(sockfd, F_GETFL, 0);
                    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

                memset(buffer, 0,sizeof(buffer));
                bytes_received = recv(sockfd, buffer, BUFF_SIZE, 0);
                printf("Recv success: socket %d, byte: %d\n", sockfd, bytes_received);

                if (bytes_received < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
                    // No data available, continue to the next socket
                    continue;
                } else if (bytes_received <= 0) {
                    FD_CLR(sockfd, &allset);
                    close(sockfd);
                    client_array[i] = -1;
                    total_client--;
                    removeNode(session_client, sockfd);
                    break;  // Connection closed or error
                }

                // Append received data to the existing partial_data
                appendText(session_client, sockfd, buffer);

                // Process the data until a "\r\n" is found
                char *crlf_position;
                while ((crlf_position = strstr(client_node->text_data, "\r\n")) != NULL) {
                    *crlf_position = '\0'; // Null-terminate the line
                    // Process the complete line

                    // Create a buffer to store the complete data
                    int buffer_full_size = client_node->text_length;
                    char *buffer_full = (char *)malloc(buffer_full_size);
                    if (buffer_full == NULL) {
                        perror("malloc");
                        // Handle memory allocation error
                    }
                    strcpy(buffer_full, client_node->text_data);
                    // Check the type of command and perform the corresponding action
                    if (strstr(buffer_full, "USER") == buffer_full) {
                        loginAccount(&client_node->login, buffer_full, buffer_full_size, buffer, sizeof(buffer), file);
                    } else if (strstr(buffer_full, "POST") == buffer_full) {
                        postMessage(client_node->login, buffer_full, buffer_full_size, buffer, sizeof(buffer));
                    } else if (strstr(buffer_full, "BYE") == buffer_full) {
                        logoutAccount(&client_node->login, buffer, sizeof(buffer));
                    } else {
                        // Unknown command
                        memset(buffer, 0, sizeof(buffer));
                        strcpy(buffer, "300");
                    }
                    send(sockfd, buffer, sizeof(buffer), 0);

                    // Move the remaining data to the beginning of the buffer
                    int remaining_length = client_node->text_length - (crlf_position - client_node->text_data) - 2;
                    memmove(client_node->text_data, crlf_position + 2, remaining_length);
                    client_node->text_length = remaining_length;
                }

                if (--nready <= 0)
                    break;
            }
        }
    }
    printf("Thread done!\n");
    free(threadParams);
    pthread_exit(NULL);
}