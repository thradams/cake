//en.cppreference.com/w/c/string/byte/isblank.html
#include <stdio.h>
#include <ctype.h>
#include <limits.h>
 
int main(void)
{
    for (int ndx=0; ndx<=UCHAR_MAX; ndx++)
        if (isblank(ndx)) printf("0x%02x\n", ndx);
}