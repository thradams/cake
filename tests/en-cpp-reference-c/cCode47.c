//en.cppreference.com/w/c/io/freopen.html
#include <stdio.h>
#include <stdlib.h>
 
int main(void)
{
    puts("stdout is printed to console");
    if (freopen("redir.txt", "w", stdout) == NULL)
    {
       perror("freopen() failed");
       return EXIT_FAILURE;
    }
    puts("stdout is redirected to a file"); // this is written to redir.txt
    fclose(stdout);
    return EXIT_SUCCESS;
}