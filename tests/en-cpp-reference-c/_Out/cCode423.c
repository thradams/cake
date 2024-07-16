//en.cppreference.com/w/c/types/max_align_t.html
#include <stdio.h>
#include <stddef.h>
#include <stdalign.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
 
int main(void)
{
    size_t a = alignof;
    printf("Alignment of max_align_t is %zu (%#zx)\n", a, a);
 
    void *p = malloc(123);
    printf("The address obtained from malloc(123) is %#" PRIxPTR"\n",
            (uintptr_t)p);
    free(p);
}