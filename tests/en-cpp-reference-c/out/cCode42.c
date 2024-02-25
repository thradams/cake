//en.cppreference.com/w/c/io/fputc.html
#include <stdio.h>
#include <stdlib.h>
 
int main(void)
{
    int ret_code = 0;
    for (char c = 'a'; (ret_code != EOF) && (c != 'z'); c++)
        ret_code = putc(c, stdout);
 
    // Test whether EOF was reached.
    if (ret_code == EOF && ferror(stdout))
    {
        perror("putc()");
        fprintf(stderr, "putc() failed in file %s at line # %d\n",
                __FILE__, __LINE__ - 7);
        exit(EXIT_FAILURE);
    }
    putc('\n', stdout);
 
    return EXIT_SUCCESS;
}