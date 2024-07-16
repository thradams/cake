//en.cppreference.com/w/c/string/byte/strdup.html
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
 
int main(void)
{
    const char *s1 = "Duplicate me!";
    char *s2 = strdup(s1);
    printf("s2 = \"%s\"\n", s2);
    free(s2);
}