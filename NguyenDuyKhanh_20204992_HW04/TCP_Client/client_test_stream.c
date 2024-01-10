//Test Stream data
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
        char filename1[256], filename2[256];
        const char *fileName1, *fileName2;
        int file_size1, file_size2;
        //printf("Enter the file path (empty input to exit): ");
        // fgets(filename, sizeof(filename), stdin);
        // filename[strcspn(filename, "\n")] = 0; // Remove newline character from input
        // if (strlen(filename) == 0)
        // {
        //     break;
        // }

        // Use realpath to get the absolute path
        strcpy(filename1, "TCP_Server/src/server.c");
        strcpy(filename2, "/mnt/c/Users/khanh/Downloads/IFFT.png");

        char absolute_path1[PATH_MAX];
        if (realpath(filename1, absolute_path1) == NULL)
        {
            perror("Error converting path");
            continue;
        }
        char absolute_path2[PATH_MAX];
        if (realpath(filename2, absolute_path2) == NULL)
        {
            perror("Error converting path");
            continue;
        }
        // Extract the file name from the absolute path
        fileName1 = basename(absolute_path1);
        fileName2 = basename(absolute_path2);
        printf("File name 1: %s\n", fileName1);
        printf("File name 2: %s\n", fileName2);

        FILE *file1 = fopen(absolute_path1, "rb");
        if (file1 == NULL)
        {
            perror("Error opening file");
            continue;
        }

        FILE *file2 = fopen(absolute_path2, "rb");
        if (file2 == NULL)
        {
            perror("Error opening file");
            continue;
        }

        fseek(file1, 0, SEEK_END);
        file_size1 = ftell(file1);
        rewind(file1);

        fseek(file2, 0, SEEK_END);
        file_size2 = ftell(file2);
        rewind(file2);

        // Tính toán độ dài cần cấp phát cho chuỗi upload_msg
        int msg_length1 = snprintf(NULL, 0, "UPLD %s", fileName1);
        int msg_length2 = snprintf(NULL, 0, " %d\r\nUPLD %s %d\r\n", file_size1, fileName2, file_size2);

        // Cấp phát động bộ nhớ cho chuỗi upload_msg
        char *upload_msg1 = (char *)malloc(msg_length1);
        char *upload_msg2 = (char *)malloc(msg_length2);

        // Kiểm tra xem cấp phát có thành công không
        if (upload_msg1 == NULL)
        {
            printf("Failed to allocate memory for upload_msg\n");
            break;
        }
        // Kiểm tra xem cấp phát có thành công không
        if (upload_msg2 == NULL)
        {
            printf("Failed to allocate memory for upload_msg\n");
            break;
        }
        // Sử dụng sprintf để điền dữ liệu vào chuỗi
        sprintf(upload_msg1, "UPLD %s", fileName1);
        sprintf(upload_msg2, " %d\r\nUPLD %s %d\r\n", file_size1, fileName2, file_size2);

        send(clientSocket, upload_msg1, strlen(upload_msg1), 0);
        send(clientSocket, upload_msg2, strlen(upload_msg2), 0);
        // Giải phóng bộ nhớ sau khi sử dụng
        free(upload_msg1);
        free(upload_msg2);

        // Receive a response from the server
        memset(buffer, 0, sizeof(buffer));
        recv(clientSocket, buffer, sizeof(buffer), 0);

        printf("Server: %s\n", buffer);

        if (strcmp(buffer, "+OK Please send file") == 0)
        {
            while (1)
            {
                memset(buffer, 0, sizeof(buffer));
                int bytes_read = fread(buffer, 1, sizeof(buffer), file1);
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

        // Receive a response from the server
        memset(buffer, 0, sizeof(buffer));
        recv(clientSocket, buffer, sizeof(buffer), 0);

        printf("Server: %s\n", buffer);

        if (strcmp(buffer, "+OK Please send file") == 0)
        {
            while (1)
            {
                memset(buffer, 0, sizeof(buffer));
                int bytes_read = fread(buffer, 1, sizeof(buffer), file2);
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

        fclose(file1);
         fclose(file2);
    }

    close(clientSocket);

    return 0;
}
