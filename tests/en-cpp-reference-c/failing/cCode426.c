//en.cppreference.com/w/c/types/ptrdiff_t.html
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
 
int main(void)
{
    const size_t N = 100;
    int numbers[N];
 
    printf("PTRDIFF_MAX = %ld\n", PTRDIFF_MAX);
    int *p1 = &numbers[18], *p2 = &numbers[23];
    ptrdiff_t diff = p2 - p1;
    printf("p2-p1 = %td\n", diff);
}