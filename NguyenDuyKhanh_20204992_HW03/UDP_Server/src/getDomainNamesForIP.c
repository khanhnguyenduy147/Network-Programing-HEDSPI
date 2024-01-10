#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include "../inc/logRequest.h"

/**
 * getDomainNamesForIP - Resolve an IP address to obtain associated domain names.
 *
 * This function takes an IP address as input and uses the getaddrinfo function to
 * resolve the IP address to obtain domain names associated with it. It supports both
 * IPv4 and IPv6 addresses. The results are stored in the `buffer` parameter in a specific format.
 *
 * @param ipAddress The IP address to be resolved.
 * @param buffer    The buffer to store the resolved domain names.
 */

void getDomainNamesForIP(const char *ipAddress, char *buffer) {
    int index = 0;
    
    // Initialize variables and structures.
    struct addrinfo hints, *res, *p;
    char ipstr[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;    // allow both IPv4 and IPv6.
    hints.ai_socktype = SOCK_STREAM;

    memset(buffer, 0, sizeof(buffer)); 

    // Use getaddrinfo to resolve the IP address to domain names.
    int status = getaddrinfo(ipAddress, NULL, &hints, &res);
    if (status != 0) {
        // printf("getaddrinfo: %s\n", gai_strerror(status));
        sprintf(buffer, "-Not found information");
        // Log the request and result
        logRequest(ipAddress, buffer);
        return;
    }

    // Iterate through the results from getaddrinfo.
    for (p = res; p != NULL; p = p->ai_next) {
        void *addr;
        char *ipver;

        // Determine whether the address is IPv4 or IPv6.
        if (p->ai_family == AF_INET) {
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
            addr = &(ipv4->sin_addr);
            ipver = "IPv4";
        } else {
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
            addr = &(ipv6->sin6_addr);
            ipver = "IPv6";
        }

        // Convert the binary address to a string.
        inet_ntop(p->ai_family, addr, ipstr, sizeof(ipstr));

        // Fing the hostname associated with the IP.
        struct hostent *host = gethostbyaddr(addr, p->ai_addrlen, p->ai_family);
        if (host != NULL) {
            if(index != 0){
                sprintf(buffer + index, "%s ", host->h_name);
                index += strlen(host->h_name) + 2;
            }else{
                sprintf(buffer + index, "+%s ", host->h_name);
                index += strlen(host->h_name) + 2;
            }
        } else {
            sprintf(buffer, "-Not found information");
        }
    }
    // Log the request and result
    logRequest(ipAddress, buffer);

    freeaddrinfo(res);
}