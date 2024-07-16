//en.cppreference.com/w/c/string/byte/isxdigit.html
#include <ctype.h>
#include <limits.h>
#include <stdio.h>
 
int main(void)
{
    for (int ndx = 0; UCHAR_MAX >= ndx; ++ndx)
        if (isxdigit(ndx))
            printf("%c", ndx);
    printf("\n");
}