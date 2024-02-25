//en.cppreference.com/w/c/io/fwrite.html
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
 
enum { SIZE = 5 };
 
int main(void)
{
    double a[SIZE] = {1, 2, 3, 4, 5};
    FILE* f1 = fopen("file.bin", "wb");
    assert(f1);
    size_t r1 = fwrite(a, sizeof a[0], SIZE, f1);
    printf("wrote %zu elements out of %d requested\n", r1, SIZE);
    fclose(f1);
 
    double b[SIZE];
    FILE* f2 = fopen("file.bin", "rb");
    size_t r2 = fread(b, sizeof b[0], SIZE, f2);
    fclose(f2);
    printf("read back: ");
    for (size_t i = 0; i < r2; ++i)
        printf("%0.2f ", b[i]);
}