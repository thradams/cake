//en.cppreference.com/w/c/string/byte/isdigit.html
#include <stdio.h>
#include <ctype.h>
#include <limits.h>
 
int main(void)
{
    for (int ndx=0; ndx<=UCHAR_MAX; ndx++)
        if (isdigit(ndx)) printf("%c", ndx);
    printf("\n");
}