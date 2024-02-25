//en.cppreference.com/w/c/types/integer.html
#include <inttypes.h>
#include <stdio.h>
 
int main(void)
{
    printf("%zu\n", sizeof(int64_t));
    printf("%s\n", PRId64);
    printf("%+" PRId64 "\n", INT64_MIN);
    printf("%+" PRId64 "\n", INT64_MAX);
 
    int64_t n = 7;
    printf("%+" PRId64 "\n", n);
}