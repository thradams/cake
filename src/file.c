#include <stdio.h>



int main(void)
{
    printf("Alignment of char = %zu\n", alignof(char));

    printf("alignof(float[10]) = %zu\n", alignof(float[10]));
    printf("alignof(struct{char c; int n;}) = %zu\n",
        alignof(struct { char c; int n; }));
}
