#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
//Get option of user
int getMenu(){
        int menu = 0;

        printf("1. Log in\n");
        printf("2. Post message\n");
        printf("3. Logout\n");
        printf("4. Exit\n");
        scanf("%d", &menu);

        return menu;
}

//Check account
int checkAccount(const char* username) {
    FILE *file = fopen("Source/account.txt", "r");

    if (file == NULL) {
        perror("Cannot open file");
        return 0; // Error opening file
    }

    char *line = NULL; 
    size_t len = 0;
    int found = 0; // The variable checks whether the login name exists in the file or not

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
    fclose(file); 

    return found;   //return 1: Valid account, return 0: The account is not exist, return 2: The account is banned
}

// Function to log activity
void logActivity(const char *function, const char *username, const char *result) {
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    FILE *logFile = fopen("Source/log_MSSV.txt", "a");
    if (logFile != NULL) {
        fprintf(logFile, "[%02d/%02d/%04d %02d:%02d:%02d] $ %s $ %s $ %s\n",
                timeinfo->tm_mday, timeinfo->tm_mon + 1, timeinfo->tm_year + 1900,
                timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec,
                function, username, result);
        fclose(logFile);
    } else {
        perror("Error opening log file");
    }
}
//
//Function of 1. Login
void loginAccount(int *login){
    char *username = NULL;
    size_t username_length = 0;

    printf("User name: ");
    while (getchar() != '\n');
    getline(&username, &username_length, stdin);
    // Delete new line
    username[strcspn(username, "\n")] = 0;

    if(*login == 1){
        printf("Error: You are signed in\n");
        logActivity("1", username, "-ERR");
    }else if(checkAccount(username) == 2){
        printf("Account is banned\n");
        logActivity("1", username, "-ERR");
    } else if(checkAccount(username) == 1){
        printf("Hello %s\n", username);
        logActivity("1", username, "+OK");
        *login = 1;
    }else{
        printf("Account is not exist\n");
        logActivity("1", username, "-ERR");
    }

    free(username);
}
//
void flushInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        // Đọc và loại bỏ các ký tự cho đến khi gặp '\n' hoặc kết thúc tập tin
    }
}

//Function of 2. Post message
void postMessage(int login){
    char message[100];
    printf("Post message: ");
    flushInputBuffer();
    fgets(message, sizeof(message), stdin);

    // Remove newline characters (if any)
    size_t len = strlen(message);
    if (len > 0 && message[len - 1] == '\n') {
        message[len - 1] = '\0'; 
    }

    if(login == 1){
        printf("Successful post\n");
        logActivity("2", message, "+OK");
    }else{
        printf("You have not logged in.\n");
        logActivity("2", message, "-ERR");
    }
}

//Function of 3. Logout 
void logoutAccount(int *login){
    if(*login == 1){
        printf("Signed out successfully\n");
        logActivity("3", "", "+OK");
        *login = 0;
    }else{
        logActivity("3", "", "-ERR");
        printf("Logout error: you are not logged in\n");
    }
}