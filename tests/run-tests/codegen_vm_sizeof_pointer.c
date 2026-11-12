/*
   sizeof of a pointer to a VLA is the size of a pointer: it has no runtime
   dimension. The generated expression used to lose its closing parenthesis.
*/

#include <stdio.h>

static int failures = 0;

#define CHECK(cond) do { if (!(cond)) { printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); failures++; } } while(0)

static unsigned long long size_of_parameter(int n, int (*p)[n])
{
    return sizeof p;
}

int main(void)
{
    int n = 3;
    int buffer[3][3] = { 0 };
    int (*p)[n] = buffer;

    unsigned long long size_of_p = sizeof p;
    unsigned long long size_of_type = sizeof(int (*)[n]);
    CHECK(size_of_p == sizeof(void*));
    CHECK(size_of_type == sizeof(void*));
    CHECK(sizeof *p == 3 * sizeof(int));
    CHECK(size_of_parameter(n, p) == sizeof(void*));

    return failures;
}
