#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
/* #include <stddef.h> */

#define PORT 28333

void respond_http(int, const char*[], size_t, const char*);

int main(void)
{
    int sd = socket(AF_INET, SOCK_STREAM, 0);

    if (sd < 0)
    {
        perror("error creating a socket");
        exit(errno);
    }

    const int y = 1;
    if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &y, sizeof(y)) < 0)
    {
        perror("Error setting option for socket");
        exit(errno);
    }

    struct sockaddr_in sd_address;
    socklen_t sd_address_size = sizeof(sd_address);

    memset(&sd_address, 0, sd_address_size);
    sd_address.sin_family = AF_INET;
    sd_address.sin_port = htons(PORT);
    sd_address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if (bind(sd, (struct sockaddr*)&sd_address, sd_address_size) < 0)
    {
        perror("bind failed");
        exit(errno);
    }

    if (listen(sd, 1) < 0)
    {
        perror("listen failed");
        exit(errno);
    }
    printf("Listening on port %d...\n", PORT);

    while (1)
    {
        printf("%s\n", "start of the loop;");
        int new_sd = accept(sd, (struct sockaddr*)&sd_address, &sd_address_size);

        const char *headers[] = {
            "HTTP/1.1 200 OK\r\n",
            "Content-Type: text/plain\r\n",
        };

        respond_http(
            new_sd,
            headers,
            sizeof(headers)/sizeof(headers[0]),
            "Hello, World, I am newbie C programmer!"
        );
    }

    shutdown(sd, SHUT_RDWR);


    exit(0);
}

void respond_http(int sd, const char *headers[], size_t h_len, const char *body)
{
    int bf_len = 0;
    int body_len = 0;

    for (int i=0; i<(int)h_len; i++)
    {
        bf_len += strlen(headers[i]);
    }

    body_len = strlen(body);
    char content_len_header[24];
    if (body_len > 0)
    {
        bf_len += body_len;
        sprintf(content_len_header, "%s: %d\r\n", "Content-Lenght", body_len);
    }


    bf_len += 25;
    char resp[bf_len];
    char *curr_pos = resp;
    size_t remaining = bf_len;

    for (int i=0;i<(int)h_len;i++)
    {
        int written = snprintf(curr_pos, remaining, "%s", headers[i]);
        if (written < 0 || written > (int)remaining)
        {
            break;
        }
        curr_pos += written;
        remaining -= written;
    }

    if (body_len > 0 && remaining > 0)
    {
        snprintf(curr_pos, remaining, "%s\r\n%s", content_len_header, body);
    }
    printf("response size: %lu\n", sizeof(resp));
    printf("created size: %d\n", bf_len);


    size_t b_sent = 0;

    do {
        b_sent += send(sd, resp, sizeof(resp), 0);
        printf("b_sent: %lu\n", b_sent);

        if (b_sent < 0)
        {
            perror("Error sending response");
            break;
        }
        if (b_sent == sizeof(resp))
        {
            printf("break");
            break;
        }
    } while (b_sent < bf_len);
    printf("%s\r", "finish");
}
