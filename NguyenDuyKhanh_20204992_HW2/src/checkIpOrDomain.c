#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "../inc/isNumericOrDot.h"
//#include "isNumericOrDot.c"

//The function receives a character string and distinguishes whether nso is an IPv4 or IPv6 address or domain name
// It returns an integer code to indicate the result.
// 1: If the input is a valid IPv4 address.
// 2: If the input is a valid IPv6 address.
// 0: If the input is a domain name.
// 3: If the input is neither a valid IP address nor a valid domain name.
int checkIpOrDomain(char *input){

    // Initialize the addrinfo structure and set the AI_CANONNAME flag
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_flags = AI_CANONNAME;

    // Create variables to store IPv4 and IPv6 addresses.
    struct in_addr ipv4;
    struct in6_addr ipv6;
    
    // Check if the input is a valid IPv4 address.
    if (inet_pton(AF_INET, input, &ipv4) != 0) {
        return 1;
    // Check if the input is a valid IPv6 address.
    } else if (inet_pton(AF_INET6, input, &ipv6) != 0) {
        return 2;
    // Use getaddrinfo to check if the input is a domain name.
    // If getaddrinfo succeeds, the input is a domain name; otherwise, it's neither a valid IP address nor a domain name.
    }else if ((getaddrinfo(input, NULL, &hints, &res) != 0) | (isNumericOrDot(input))) {
        return 0;
    } else {
        return 3;
    }
}