#include <stdio.h>
#include <string.h>
#include <time.h>

//Function of 3. Logout 
void logoutAccount(int *login){
    if(*login == 1){
        printf("Signed out successfully\n");
        logActivity("3", "", "+OK");
        *login = 0;
    }else{
        logActivity("3", "", "-ERR");
        perror("Error: you are not logged in");
    }
}