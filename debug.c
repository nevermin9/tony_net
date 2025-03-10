#include <unistd.h>
#include <stdio.h>


int
main()
{
    int var = 2;
    printf("Welcome to the prog with a bug!\n");
    scanf("%d", var);
    printf("you gave me: %d\n", var);
    return 0;
}
