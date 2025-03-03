#include <math.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define INIT_LEN 100

typedef struct {
    size_t len;
    char *headers;
} HTTPHeaders;
int create_http_headers(HTTPHeaders*, size_t, ...);

int main(void)
{
    printf("%d", (int)floor(log10(abs(123))) + 1);

}





