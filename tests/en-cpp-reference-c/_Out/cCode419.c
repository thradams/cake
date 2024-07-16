//en.cppreference.com/w/c/types/NULL.html
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
 
int main(void)
{
    // any kind of pointer can be set to NULL
    int* p = NULL;
    struct S *s = NULL;
    void(*f)(int, double) = NULL;
    printf("%p %p %p\n", (void*)p, (void*)s, (void*)(long)f);
 
    // many pointer-returning functions use null pointers to indicate error
    char *ptr = malloc(0xFULL);
    if (ptr == NULL)
        printf("Out of memory");
    else
        printf("ptr = %#" PRIxPTR"\n", (uintptr_t)ptr);
    free(ptr);
}