//en.cppreference.com/w/c/program/at_quick_exit.html
/* tcc headers (msvcrt) lack what this sample uses */
#ifndef __TINYC__
#include <stdlib.h>
#include <stdio.h>
 
void f1(void)
{
    puts("pushed first");
    fflush(stdout);
}
 
void f2(void)
{
    puts("pushed second");
}
 
int main(void)
{
    at_quick_exit(f1);
    at_quick_exit(f2);
    quick_exit(0);
}
#endif
