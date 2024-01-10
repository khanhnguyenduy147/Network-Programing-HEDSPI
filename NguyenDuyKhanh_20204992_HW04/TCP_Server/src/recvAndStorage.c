#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

#define BUFFER_SIZE 2048

/**
 * recvAndStorage - Receive data from a client and store it in a file.
 *
 * This function is responsible for receiving data from a client over a socket and
 * storing it in a file. It ensures that the received data is written to the file until
 * the specified file size is reached.
 *
 * @param filename      The name of the file where data will be stored.
 * @param file_size     The size of the file to be received.
 * @param clientSocket  The socket connected to the client.
 * @param received      A pointer to an integer that tracks the number of bytes received.
 */

void recvAndStorage(char filename[], int file_size, int clientSocket, int *received){
            char buffer[BUFFER_SIZE];
            *received = 0;

            // Open the file for writing
            FILE *file = fopen(filename, "wb");
            if (file == NULL) {
                perror("Error opening storage file");
                return;
            }

            while (*received < file_size) {
                memset(buffer, 0, sizeof(buffer));
                int bytes_received = recv(clientSocket, buffer, sizeof(buffer), 0);
                // Check for errors or the end of data
                if (bytes_received <= 0) {
                    return;
                }
                // Write the received data to the file
                fwrite(buffer, 1, bytes_received, file);
                *received += bytes_received;
            }

            // Close the file when all data is received
            fclose(file);
}