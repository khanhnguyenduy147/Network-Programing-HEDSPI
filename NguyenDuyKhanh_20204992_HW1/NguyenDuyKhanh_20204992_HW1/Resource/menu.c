#include "../Header/menu.h"
#include "login.c"
#include "logout.c"
#include "postmessage.c"
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

    char line[100]; // Maximum length of each line
    int found = 0; // The variable checks whether the login name exists in the file or not

    while (fgets(line, sizeof(line), file)) {
        char storedUsername[50];
        int status;
        if (sscanf(line, "%s %d", storedUsername, &status) == 2) {
            if (strcmp(username, storedUsername) == 0 && status == 1) {
                found = 1;
                break; // Found valid account, exit the loop
            }
        }
    }

    fclose(file); 

    return found;   //return 1: Valid account, return 0: The account is invalid or does not exist
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
