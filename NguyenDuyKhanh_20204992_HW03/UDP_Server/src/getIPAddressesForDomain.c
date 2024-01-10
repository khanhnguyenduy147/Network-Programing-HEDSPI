#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include "../inc/logRequest.h"

#define BUF_SIZE 1024

/**
 * getIPAddressesForDomain - Resolve a domain name to obtain its IP addresses.
 *
 * This function takes a domain name as input and uses the getaddrinfo function to
 * resolve the domain name to obtain its IP addresses. It supports both IPv4 and IPv6
 * addresses. The results are stored in the `buffer` parameter in a specific format.
 *
 * @param domainName The domain name to be resolved.
 * @param buffer      The buffer to store the resolved IP addresses.
 */

void getIPAddressesForDomain(const char *domainName, char *buffer) {
    int index = 0;

    struct addrinfo hints, *res, *p;
    char ipstr[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;    //allow both IPV4 and IPv6
    hints.ai_socktype = SOCK_STREAM;

    memset(buffer, 0, sizeof(buffer)); 
    // Use getaddrinfo to resolve the domain name to IP addresses
    int status = getaddrinfo(domainName, NULL, &hints, &res);
    if (status != 0) {
        sprintf(buffer, "-Not found information");
        logRequest(domainName, buffer);
        return;
    }

    // Use getaddrinfo to resolve the domain name to IP addresses
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
        if(index != 0){
            sprintf(buffer + index, "%s ", ipstr);
            index += strlen(ipstr) + 2;
        }else{
            sprintf(buffer + index, "+%s ", ipstr);
            index += strlen(ipstr) + 2;
        }
    }

    // Log the request and result
    logRequest(domainName, buffer);

    freeaddrinfo(res);
}