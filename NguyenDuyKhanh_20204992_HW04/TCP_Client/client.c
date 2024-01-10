#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <limits.h>
#include <libgen.h> // Include the library for using basename

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <IP_Addr> <Port_Number>\n", argv[0]);
        exit(1);
    }

    int clientSocket;
    struct sockaddr_in serverAddr;
    char buffer[1024];

    // Create a socket
    if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Error creating socket");
        exit(1);
    }

    // Configure the server address
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(atoi(argv[2]));
    inet_pton(AF_INET, argv[1], &(serverAddr.sin_addr));

    // Connect to the server
    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
    {
        perror("Error connecting to the server");
        exit(1);
    }

    // Receive a welcome message from the server
    memset(buffer, 0, sizeof(buffer));
    recv(clientSocket, buffer, sizeof(buffer), 0);
    printf("Server: %s\n", buffer);

    while (1)
    {
        char filename[256];
        const char *fileName;
        int file_size;
        printf("Enter the file path (empty input to exit): ");
        fgets(filename, sizeof(filename), stdin);
        filename[strcspn(filename, "\n")] = 0; // Remove newline character from input
        if (strlen(filename) == 0)
        {
            break;
        }

        // Use realpath to get the absolute path
        char absolute_path[PATH_MAX];
        if (realpath(filename, absolute_path) == NULL)
        {
            perror("Error converting path");
            continue;
        }

        // Extract the file name from the absolute path
        fileName = basename(absolute_path);
        printf("File name: %s\n", fileName);

        FILE *file = fopen(absolute_path, "rb");
        if (file == NULL)
        {
            perror("Error opening file");
            continue;
        }

        fseek(file, 0, SEEK_END);
        file_size = ftell(file);
        rewind(file);

        // Calculate the length to allocate for the upload_msg string
        int msg_length = snprintf(NULL, 0, "UPLD %s %d", fileName, file_size) + 3;

        // Dynamically allocate memory for upload_msg string
        char *upload_msg = (char *)malloc(msg_length);

        // Check if allocation was successful
        if (upload_msg == NULL)
        {
            printf("Failed to allocate memory for upload_msg\n");
            break;
        }
        // Use sprintf to fill the string with data
        sprintf(upload_msg, "UPLD %s %d\r\n", fileName, file_size);

        send(clientSocket, upload_msg, strlen(upload_msg), 0);
        // Free memory after use
        free(upload_msg);

        // Receive a response from the server
        memset(buffer, 0, sizeof(buffer));
        recv(clientSocket, buffer, sizeof(buffer), 0);

        printf("Server: %s\n", buffer);

        if (strcmp(buffer, "+OK Please send file") == 0)
        {
            while (1)
            {
                memset(buffer, 0, sizeof(buffer));
                int bytes_read = fread(buffer, 1, sizeof(buffer), file);
                if (bytes_read == 0)
                {
                    break;
                }
                send(clientSocket, buffer, bytes_read, 0);
            }

            // Receive a completion response from the server
            memset(buffer, 0, sizeof(buffer));
            recv(clientSocket, buffer, sizeof(buffer), 0);
            printf("Server: %s\n", buffer);
        }

        fclose(file);
    }

    close(clientSocket);

    return 0;
}
