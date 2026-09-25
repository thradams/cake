//en.cppreference.com/w/c/language/_Alignof.html
#include <stdalign.h>
#include <stddef.h>
#include <stdio.h>
 
int main(void)
{
    printf("Alignment of char = %zu\n", alignof(char));
#ifndef _WIN32 /* MSVC has no max_align_t */
    printf("Alignment of max_align_t = %zu\n", alignof(max_align_t));
#endif
    printf("alignof(float[10]) = %zu\n", alignof(float[10]));
    printf("alignof(struct{char c; int n;}) = %zu\n",
            alignof(struct {char c; int n;}));
}