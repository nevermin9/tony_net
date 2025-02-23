#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
/* #include <stddef.h> */

int main()
{
    int sd = socket(AF_INET, SOCK_STREAM, 0);

    if (sd < 0)
    {
        printf("Error: %d", errno);
        return errno;
    }

    const int y = 1;
    if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &y, sizeof(y)) < 0)
    {
        perror("Error setting option for socket");
        return errno;
    }

    struct sockaddr_in sd_address;
    memset(&sd_address, 0, sizeof(sd_address));
    sd_address.sin_family = AF_INET;
    sd_address.sin_port = htons(28333);
    sd_address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    socklen_t sd_address_size = sizeof(sd_address);
    if (bind(sd, (struct sockaddr*)&sd_address, sd_address_size) < 0)
    {
        perror("bind failed");
        return errno;
    }

    if (listen(sd, 1) < 0)
    {
        perror("listen failed");
        return errno;
    }

    while (1)
    {
        int new_sd = accept(sd, (struct sockaddr*)&sd_address, &sd_address_size);
        printf("%d\n", new_sd);
        break;
    }

    shutdown(sd, SHUT_RDWR);



    printf("%s\n", "Hello, World!");
    return 0;
}
