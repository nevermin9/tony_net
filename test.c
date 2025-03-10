#include <stdarg.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

int 
main(int argc, char **argv)
{
    struct addrinfo hints;
    struct addrinfo *res;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    /* hints.ai_family = AF_INET6; */
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_protocol = 0;
    hints.ai_addr = NULL;
    hints.ai_canonname = NULL;
    hints.ai_next = NULL;

    int s = getaddrinfo(NULL, argv[1], &hints, &res);

    if (s != 0)
    {
        fprintf(stderr, "%s\n", "Error getaddrinfo");
        return -1;
    }

    struct addrinfo *rp;
    for (rp = res; rp != NULL; rp = res->ai_next)
    {
        printf("%s\n","new one");
        printf("ai_family: %d\n", rp->ai_family);
        printf("ai_socktype: %d\n", rp->ai_socktype);
        printf("ai_flags: %d\n", rp->ai_flags);
        printf("ai_protocol: %d\n", rp->ai_protocol);
        printf("ai_addr: %d\n", rp->ai_addr->sa_family);
        /* char ip_addr_repr[INET6_ADDRSTRLEN]; */
        char ip_addr_repr[INET_ADDRSTRLEN];
        inet_ntop(rp->ai_family, rp->ai_addr, ip_addr_repr, INET_ADDRSTRLEN);
        printf("ai_addr: %s\n", ip_addr_repr);
        printf("ai_canonname: %s\n", rp->ai_canonname);
    }
    char host[NI_MAXHOST];


    return 0;
}





