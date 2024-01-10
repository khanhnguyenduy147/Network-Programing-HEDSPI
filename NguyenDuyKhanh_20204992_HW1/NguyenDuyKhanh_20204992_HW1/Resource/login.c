#include <stdio.h>
#include <string.h>
#include <time.h>

//Function of 1. Login
void loginAccount(int *login){
    char username[100];

    printf("User name: ");
    scanf("%s", username);

    if((checkAccount(username) == 1) && (*login == 0)){
        printf("Account is banned\n");
        logActivity("1", username, "+OK");
        *login = 1;
    }else if((checkAccount(username) == 1) && (*login == 1)){
        printf("You are signed in\n");
        logActivity("1", username, "-ERR");
    }else{
        printf("Account is not exist\n");
        logActivity("1", username, "-ERR");
    }
}