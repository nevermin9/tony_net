#include <stdio.h>
/* #include <stdlib.h> */
#include <string.h>

int main(void)
{
    /* scanf("%s", in); */
    const char *header[] = {"anton", "nim", "moon", "desi"};
    int h_len = sizeof(header)/sizeof(header[0]);

    unsigned long bf_len = 0;
    for (int i =0;i<h_len;++i)
    {
        bf_len += strlen(header[i]);
    }

    printf("%lu\n", bf_len);
    char resp[bf_len+1];
    char *current_pos = resp;
    size_t remaining_len = bf_len+1;

    for (int i=0;i<h_len;i++)
    {
        int written = snprintf(current_pos,  remaining_len, "%s", header[i]);
        if (written < 0 || written > (int)remaining_len)
        {
            break;
        }
        current_pos += written;
        remaining_len -= written;
    }

    printf("%s\n", resp);
}
