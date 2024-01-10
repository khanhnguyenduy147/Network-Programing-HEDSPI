#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "../inc/checkIpOrDomain.h"
#include "../inc/getDomainNamesForIP.h"
#include "../inc/getIPAddressesForDomain.h"

int main() {
    char input[2048];

    printf("Enter request (Syntax: './resolver content' ):");
    //Read data and print data read error message if any
    if (fgets(input, sizeof(input), stdin) == NULL) {
        printf("Error reading data.\n");
        return 1;
    }

    // Remove newline character if present
    size_t len = strlen(input);
    if (len > 0 && input[len - 1] == '\n') {
        input[len - 1] = '\0';
    }

    // Check the syntax
    if (strncmp(input, "./resolver ", 11) != 0) {
        printf("Error: Incorrect syntax. Use './resolver content'.\n");
        return 1;
    }

    //input_new is the variable that stores the content string after removing './resolver'
    char *input_new = input + 11;

    //Prints an error message if the content is empty
    if (strlen(input_new) == 0) {
        printf("Error: Content cannot be empty.\n");
        return 1;
    }

    //Check if the content is an IPv4 or IPv6 address or a domain name
    //Returns 1 if IPv4, returns 2 if IPv6, returns 3 if domain name, returns 0 if invalid
    int check = checkIpOrDomain(input_new);

    //Based on the check value, call appropriate processing functions
    switch (check){
        case 0:
            printf("Not found information\n");
            break;
        case 1:
            getDomainNamesForIP(input_new);
            break;

        case 2:
            getDomainNamesForIP(input_new);
            break;
            
        case 3:
            getIPAddressesForDomain(input_new);
            break;

        default:
            break;
    }

    return 0;
}
