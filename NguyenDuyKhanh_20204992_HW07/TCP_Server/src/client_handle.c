#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <pthread.h>
#include <sys/types.h>
#include "../inc/limit_client.h"
#include "../inc/client_handle_thread.h"
#include "../inc/menu.h"
#include "../inc/linklist_data_stream.h"

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

void clientHandle(FILE *file, int listen_fd, struct LinkedList session_client){
    int i, maxi, maxfd, connfd, sockfd;
    int nready, client_array[FD_SETSIZE];
    fd_set readfds, allset;

    int conn_fd;
    struct sockaddr_in client;
    socklen_t client_len;
    char buffer[BUFF_SIZE];

    pthread_t tid;

    // Initialization
    maxfd = listen_fd;
    maxi = -1;
    for (i = 0; i < FD_SETSIZE; i++){
        client_array[i] = -1;
    }
    FD_ZERO(&allset);
    FD_SET(listen_fd, &allset);

    while (1) {
        readfds = allset;
        nready = select(maxfd + 1, &readfds, NULL, NULL, NULL);

        if (nready < 0) {
            perror("\nError in select");
            return 0;
        }

        // Handle new client connections
        if (FD_ISSET(listen_fd, &readfds)) {
            client_len = sizeof(client);
            if ((conn_fd = accept(listen_fd, (struct sockaddr*)&client, &client_len)) < 0)
                perror("\nError accepting connection");
            else {
                // Connected with client reply
                memset(buffer, 0, sizeof(buffer));
                strcpy(buffer, "100");
                send(conn_fd, buffer, sizeof(buffer), 0);

                char ip_str[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &(client.sin_addr), ip_str, INET_ADDRSTRLEN);
                //Save client accept into client array
                for (i = 0; i < FD_SETSIZE; i++){
                    if (client_array[i] < 0) {
                        client_array[i] = conn_fd;
                        break;
                    }
                }
                // Handle the case where the number of clients is too large, a new thread will be created to handle it
                if (i == FD_SETSIZE) {
                    int client_array_param[FD_SETSIZE + 2];
                    for(int i = 0; i < FD_SETSIZE; i++){
                        client_array_param[i] = client_array[i];
                    }
                    struct ThreadParams *params = malloc(sizeof(struct ThreadParams));
                    params->allset = allset;
                    params->client_array = client_array_param;
                    params->file = file;
                    params->session_client = &session_client;
                    params->listen_fd = listen_fd;
                    params->maxfd = maxfd;
                    params->maxi = maxi;
                    pthread_create(&tid, NULL, &clientHandleThread, (void *)params);
                    // Detach the thread to automatically clean up resources
                    pthread_detach(tid);
                    // Reset variables for new connections
                    maxfd = listen_fd;
                    maxi = -1;
                    for (int i = 0; i < FD_SETSIZE; i++){
                        client_array[i] = -1;
                    }
                    FD_ZERO(&allset);
                    FD_SET(listen_fd, &allset);
                    continue;
                }
                // Update the sets and variables
                FD_SET(conn_fd, &allset);
                if (conn_fd > maxfd)
                    maxfd = conn_fd;
                if (i > maxi)
                    maxi = i;

                if (--nready <= 0)
                    continue;
            }
        }
         // Handle data from existing clients
        for (i = 0; i <= maxi; i++) {
            if ((sockfd = client_array[i]) < 0)
                continue;

            if (FD_ISSET(sockfd, &readfds)) {
                int login = 0;
                int bytes_received;
                char *partial_data = NULL;  
                int partial_data_length = 0;
                struct Node *client_node;
                // If the client is not in the session list, add it
                if(!searchNode(&session_client, sockfd)){
                    addNode(&session_client, login, partial_data, partial_data_length, sockfd);
                }

                client_node = findNode(&session_client, sockfd);

                memset(buffer, 0,sizeof(buffer));
                bytes_received = recv(sockfd, buffer, BUFF_SIZE, 0);

                if (bytes_received <= 0) {
                    // Connection closed or error
                    FD_CLR(sockfd, &allset);
                    close(sockfd);
                    client_array[i] = -1;
                    removeNode(&session_client, sockfd);
                    break; 
                }

                // Append received data to the existing partial_data
                appendText(&session_client, sockfd, buffer);

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
}