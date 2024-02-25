//en.cppreference.com/w/c/language/_Alignof.html
#include <stdalign.h>
#include <stddef.h>
#include <stdio.h>
 
int main(void)
{
    printf("Alignment of char = %zu\n", alignof);
    printf("Alignment of max_align_t = %zu\n", alignof3);
    printf("alignof(float[10]) = %zu\n", alignof);
    printf("alignof(struct{char c; int n;}) = %zu\n",
            alignof);
}