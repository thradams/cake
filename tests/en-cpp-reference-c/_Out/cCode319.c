//en.cppreference.com/w/c/string/byte/isspace.html
#include <ctype.h>
#include <limits.h>
#include <stdio.h>
 
int main(void)
{
    for (int ndx = 0; ndx <= UCHAR_MAX; ndx++)
        if (isspace(ndx))
            printf("0x%02x ", ndx);
}