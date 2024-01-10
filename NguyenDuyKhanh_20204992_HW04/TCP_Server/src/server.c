#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <limits.h>
#include <errno.h>
#include "../inc/createArchiveDir.h"
#include "../inc/recvAndStorage.h"
#include "../inc/logRequest.h"

#define MAX_FILE_SIZE 4294967296 // 2^32 bytes
#define BUFFER_SIZE 2048

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <PortNumber> <Directory_name>\n", argv[0]);
        exit(1);
    }

    int serverSocket, clientSocket;
    int bytes_received;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrLen = sizeof(clientAddr);
    char buffer[BUFFER_SIZE];
    char result_log[2048];
    char request[1024];
    char result[1024];

    // Create a socket
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Error creating socket");
        exit(1);
    }

    // Configure the server address
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(atoi(argv[1]));
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Bind socket
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
    {
        perror("Error binding");
        exit(1);
    }

    // Listen for connections from clients
    if (listen(serverSocket, 1) == -1)
    {
        perror("Error listening");
        exit(1);
    }

    printf("Waiting for connections from clients...\n");

    while (1)
    { // Infinite loop to maintain connection listening
        // Accept a connection from a client
        if ((clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &addrLen)) == -1)
        {
            perror("Error accepting connection");
            continue; // Continue the loop to accept new connections
        }

        printf("Connect to client via socket: %d\n", clientSocket);

        // Send a welcome message
        send(clientSocket, "+OK Welcome to file server", strlen("+OK Welcome to file server"), 0);

        // Log the request
        memset(result_log, 0, sizeof("+OK Welcome to file server"));
        strcpy(result_log, "+OK Welcome to file server");
        logRequest(inet_ntoa(clientAddr.sin_addr), clientAddr.sin_port, result_log);

        // Handle Stream data
        char *partial_data = NULL;
        int partial_data_length = 0;

        while (1)
        { // Infinite loop to handle the client
            // Receive the filename and file size from the client
            memset(buffer, 0, sizeof(buffer));
            bytes_received = recv(clientSocket, buffer, sizeof(buffer), 0);

            if (strcmp(buffer, "") == 0)
            {
                // Receive the filename and file size from the client
                close(clientSocket);
                break;
            }

            // Append received data to the existing partial_data
            partial_data = realloc(partial_data, partial_data_length + bytes_received);
            if (partial_data == NULL)
            {
                // Handle memory allocation error
                break;
            }
            memcpy(partial_data + partial_data_length, buffer, bytes_received);
            partial_data_length += bytes_received;

            // Process the data until a "\r\n" is found
            char *crlf_position;
            while ((crlf_position = strstr(partial_data, "\r\n")) != NULL)
            {
                *crlf_position = '\0'; // Null-terminate the line
                // Process the complete line

                // Create a buffer to store the complete data
                int buffer_full_size = partial_data_length;
                char *buffer_full = (char *)malloc(buffer_full_size);
                if (buffer_full == NULL)
                {
                    perror("malloc");
                    // Handle memory allocation error
                }
                strcpy(buffer_full, partial_data);

                char filename[256];
                int file_size;
                sscanf(buffer_full, "UPLD %s %d", filename, &file_size);

                // Send a response
                send(clientSocket, "+OK Please send file", strlen("+OK Please send file"), 0);

                // Log the request
                memset(result_log, 0, sizeof("+OK Please send file"));
                strcpy(result_log, "+OK Please send file");
                logRequest(inet_ntoa(clientAddr.sin_addr), clientAddr.sin_port, result_log);
                if (file_size > MAX_FILE_SIZE)
                {
                    perror("File size exceeds the limit");
                    close(clientSocket);
                    break;
                }

                // Create the storage directory
                char root_path[PATH_MAX];
                createArchiveDir(argv[2], root_path);

                // Change to the storage directory
                if (chdir(argv[2]) == -1)
                {
                    perror("Error changing to the storage directory");
                    close(clientSocket);
                    break;
                }

                // Server receives and stores data
                int received;
                recvAndStorage(filename, file_size, clientSocket, &received);

                // Change back to the root directory
                if (chdir(root_path) == -1)
                {
                    perror("Error changing to the storage directory");
                    close(clientSocket);
                    break;
                }

                if (received == file_size)
                {
                    send(clientSocket, "+OK Successful upload", strlen("+OK Successful upload"), 0);
                    memset(result, 0, sizeof(result));
                    strcpy(result, "+OK Successful upload");
                }
                else
                {
                    perror("Error receiving the file from the client");
                    send(clientSocket, "-ERROR", strlen("-ERROR"), 0);
                    memset(result, 0, sizeof(result));
                    strcpy(result, "-ERROR");
                }
                snprintf(result_log, sizeof(result_log), "%s$%s", request, result);

                // Log the request
                logRequest(inet_ntoa(clientAddr.sin_addr), clientAddr.sin_port, result_log);

                // Move the remaining data to the beginning of the buffer
                int remaining_length = partial_data_length - (crlf_position - partial_data) - 2;
                memmove(partial_data, crlf_position + 2, remaining_length);
                partial_data_length = remaining_length;
            }
        }
        printf("Disconnect from the client %d\n", clientSocket);
    }

    close(serverSocket);

    return 0;
}
