#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdlib.h>


int main(int argc, char **argv)
{
    struct addrinfo hints = {
        .ai_family = AF_UNSPEC,
        .ai_socktype = SOCK_STREAM,
        .ai_flags = AI_PASSIVE,
        /* .ai_flags = AI_PASSIVE | AI_NUMERICSERV, */
        /* .ai_flags = AI_NUMERICSERV, */
    };
    struct addrinfo *res, *p;

    int status = getaddrinfo(NULL, "8080", &hints, &res);
    /* int status = getaddrinfo(NULL, "8080", &hints, &res); */
    if (status != 0)
    {
        fprintf(stderr, "getaddrinfo err");
        exit(errno);
    }

    for (p = res; p != NULL; p = p->ai_next)
    {
        char addrs[INET6_ADDRSTRLEN];
        int af;
        void *net_addr;
        uint32_t port;

        if (p->ai_addr->sa_family == AF_INET)
        {
            af = AF_INET;
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
            net_addr = &(ipv4->sin_addr);
            port = ntohs(ipv4->sin_port);
            printf("IPV4 for %s: ", argv[1]);
        }
        else
        {
            af = AF_INET6;
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
            net_addr = &(ipv6->sin6_addr);
            port = ntohs(ipv6->sin6_port);
            
            printf("IPV6 for %s: ", argv[1]);
        }
        printf("addrlen: %d\n",p->ai_addrlen);
        inet_ntop(af, net_addr, addrs, INET6_ADDRSTRLEN);
        printf("%s:%d\n", addrs, port);
    }

    // finish
    freeaddrinfo(res);
}
