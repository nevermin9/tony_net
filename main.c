#include <math.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>


#define PORT "28333"
#define CONTENT_LENGTH_HEADER_LEN 16

typedef struct {
    size_t len;
    char *headers;
} HTTPHeaders;

typedef struct {
    int sd;
    struct sockaddr_in *sd_addr;
    size_t sd_addrlen;
} ServerInfo;

int create_socket(ServerInfo*); 
int respond_http(int sd, HTTPHeaders*, ...);
int create_http_headers(HTTPHeaders*, size_t, ...);
size_t count_digits_num(int a);

int main(void)
{
    int status;
    struct sockaddr_storage their_addr;
    socklen_t their_addr_len;
    char sock_addr_repr[INET6_ADDRSTRLEN];
    ServerInfo server_info;

    status = create_socket(&server_info);

    printf("Server info: \n");
    inet_ntop(server_info.sd_addr->sin_family, &( server_info.sd_addr->sin_addr ), sock_addr_repr, INET6_ADDRSTRLEN);
    printf("Address: %s\n", sock_addr_repr);
    printf("Port: %d\n", ntohs(server_info.sd_addr->sin_port));

    if (status < 0)
    {
        exit(errno);
    }

    status = bind(server_info.sd, (struct sockaddr *)server_info.sd_addr, server_info.sd_addrlen);

    if (status < 0)
    {
        perror("bind failed");
        exit(errno);
    }

    status = listen(server_info.sd, 20);

    if (status < 0)
    {
        perror("listen failed");
        exit(errno);
    }

    printf("Listening on port %s...\n", PORT);

    their_addr_len = sizeof(their_addr);
    for (;;)
    {
        int new_sd = accept(server_info.sd, (struct sockaddr*)&their_addr, &their_addr_len);

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

    shutdown(server_info.sd, SHUT_RDWR);

    exit(0);
}

int create_socket(ServerInfo *server_info)
{
    struct addrinfo hints = {
        /* .ai_family = AF_UNSPEC, */
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM,
        .ai_flags = AI_NUMERICSERV,
    };
    struct addrinfo *res, *p;
    int status;

    status = getaddrinfo(NULL, PORT, &hints, &res);

    if (status != 0)
    {
        perror("error getaddrinfo\n");
        return -1;
    }

    for (p = res; p != NULL; p = p->ai_next)
    {
        if (p->ai_family == AF_INET)
        {
            break;
        }
    }


    if (p == NULL)
    {
        perror("failed in creating addrinfo");
        return -1;
    }

    int sd = socket(p->ai_family, p->ai_socktype, 0);

    if (sd < 0)
    {
        perror("creating the socket\n");
        return sd;
    }

    if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
    {
        perror("error setting socket options");
        return -1;
    }

    server_info->sd = sd;
    server_info->sd_addrlen = p->ai_addrlen;

    server_info->sd_addr = malloc(server_info->sd_addrlen);
    if (server_info->sd_addr == NULL)
    {
        perror("malloc failed\n");
        return -1;
    }

    memcpy(server_info->sd_addr, p->ai_addr, server_info->sd_addrlen);

    return 0;
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
