#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>

//// This function receives a domain name and resolves it to obtain its IP addresses.
void getIPAddressesForDomain(const char *domainName) {
    struct addrinfo hints, *res, *p;
    char ipstr[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;    //allow both IPV4 and IPv6
    hints.ai_socktype = SOCK_STREAM;

    // Use getaddrinfo to resolve the domain name to IP addresses
    int status = getaddrinfo(domainName, NULL, &hints, &res);
    if (status != 0) {
        printf("Not found information\n");
        return;
    }

    printf("Result:\n");
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
        printf("%s: %s\n", ipver, ipstr);
    }

    freeaddrinfo(res);
}