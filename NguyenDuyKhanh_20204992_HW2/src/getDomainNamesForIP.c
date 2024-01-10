#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>

//Ham receives the IP character string and resolves the domain names.
void getDomainNamesForIP(const char *ipAddress) {

    // Initialize variables and structures.
    struct addrinfo hints, *res, *p;
    char ipstr[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;    // allow both IPv4 and IPv6.
    hints.ai_socktype = SOCK_STREAM;

    // Use getaddrinfo to resolve the IP address to domain names.
    int status = getaddrinfo(ipAddress, NULL, &hints, &res);
    if (status != 0) {
        // printf("getaddrinfo: %s\n", gai_strerror(status));
        printf("Not found information\n");
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
        printf("IP Address: %s\n", ipstr);

        // Fing the hostname associated with the IP.
        struct hostent *host = gethostbyaddr(addr, p->ai_addrlen, p->ai_family);
        if (host != NULL) {
            printf("Hostname: %s\n", host->h_name);
        } else {
            printf("Not found information\n");
        }
    }

    freeaddrinfo(res);
}