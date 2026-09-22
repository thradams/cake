/*
   C23 6.9.1: parameters of a function definition may be unnamed. The
   generated code targets older compilers, so cake has to give them a name.
   Prototypes and function pointer types keep their unnamed parameters.
*/
#include <stdio.h>

static int failures = 0;

#define CHECK(cond) do { if (!(cond)) { printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); failures++; } } while(0)

int f(int, char*, int (*)(int, int));
static int g(int, double);
int (*fp)(int, int);

int f(int, char*, int (*)(int, int))
{
    return 1;
}

static int g(int, double)
{
    return 2;
}

int h(int a, int, int c)
{
    return a + c;
}

int main(void)
{
    CHECK(f(0, "", 0) == 1);
    CHECK(g(0, 0) == 2);
    CHECK(h(1, 100, 2) == 3);
    CHECK(fp == 0);
    return failures;
}
