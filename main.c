#include <math.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
/* #include <stddef.h> */

#define PORT 28333
#define CONTENT_LENGTH_HEADER_LEN 16

typedef struct {
    size_t len;
    char *headers;
} HTTPHeaders;

int respond_http(int sd, HTTPHeaders*, ...);
int create_http_headers(HTTPHeaders*, size_t, ...);
size_t count_digits_num(int a);

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
        int new_sd = accept(sd, (struct sockaddr*)&sd_address, &sd_address_size);

        // response
        char *body = "Hi, my name is Anton and I want to be a hacker!";
        uint32_t body_size = strlen(body);
        uint32_t len = CONTENT_LENGTH_HEADER_LEN+count_digits_num(body_size) + 1;
        char *content_legth_header = calloc(len, sizeof(char));
        snprintf(content_legth_header, len, "%s%d", "Content-Length: ", body_size);

        HTTPHeaders headers = {.len=0, .headers=NULL};
        create_http_headers(
            &headers, 3,
            "HTTP/1.1 200 OK",
            "Content-Type: text/plain",
            content_legth_header
        );
        respond_http(new_sd, &headers, body);

        free(content_legth_header);
    }

    shutdown(sd, SHUT_RDWR);

    exit(0);
}

size_t count_digits_num(int a)
{
    return (uint32_t)floor(log10(abs(a))) + 1;
}


int create_http_headers(HTTPHeaders *hdrs, size_t h_count, ...)
{
    if (hdrs->headers != NULL)
    {
        printf("Headers are already filled!\n");
        return -1;
    }

    va_list args1;
    va_start(args1, h_count);
    va_list args2;
    va_copy(args2, args1);

    for (uint32_t i = 0; i < h_count; i++)
    {
        char *h = va_arg(args1, char*);
        hdrs->len += strlen(h);
    }
    // clean up args1
    va_end(args1);

    // count \r\n after each header
    // + last \r\n
    // + \0
    hdrs->len += ((h_count * 2) + 3);

    hdrs->headers = calloc(hdrs->len, sizeof(char));
    char *curr_pos = hdrs->headers;
    uint32_t remaining = hdrs->len;
    uint32_t written = 0;

    for (uint32_t i = 0; i < h_count; i++)
    {
        char *h = va_arg(args2, char*);
        written = snprintf(curr_pos, remaining, "%s\r\n", h);
        if (written < 0 || written > remaining)
        {
            printf("Buffer overflow!\n");
            return -1;
        }
        curr_pos += written;
        remaining -= written;
    }
    va_end(args2);

    snprintf(curr_pos, remaining, "%s", "\r\n");

    return 0;
}


int respond_http(int sd, HTTPHeaders* headers, ...)
{
    va_list args;
    va_start(args, headers);
    char *body = va_arg(args, char*);
    va_end(args);

    uint32_t resp_len = headers->len + strlen(body) + 1;
    char *response = calloc(resp_len, sizeof(char));
    snprintf(response, resp_len, "%s%s", headers->headers, body);

    uint32_t bytes_sent = 0;

    do {
        bytes_sent += send(sd, response, resp_len, 0);

        if (bytes_sent < 0)
        {
            free(response);
            printf("Error sending response!\n");
            return -1;
        }
    } while (bytes_sent < resp_len);

    free(response);
    return 0;
}
