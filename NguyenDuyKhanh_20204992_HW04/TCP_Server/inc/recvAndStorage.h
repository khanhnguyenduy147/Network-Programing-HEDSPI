#ifndef RECVANDSTORAGE_H
#define RECVANDSTORAGE_H

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

void recvAndStorage(char filename[], int file_size, int clientSocket, int *received);
#endif