/*
   Code that an expression needs before its statement (the body of a
   statement expression, a temporary...) is "hoisted" by codegen. It has to run
   only when the operand that produced it is evaluated: the right side of &&
   and ||, and the branches of ?:. Two statement expressions in the same
   expression must both keep their hoisted code.
*/

#include <stdio.h>

static int failures = 0;

#define CHECK(cond) do { if (!(cond)) { printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); failures++; } } while(0)

static int calls = 0;

static int f(int x)
{
    calls++;
    return x;
}

static int id(int x)
{
    return x;
}

int main(void)
{
    int zero = id(0);
    int one = id(1);

    /* && : the right side is not evaluated */
    calls = 0;
    int r = zero && ({ int t = f(1); t; });
    CHECK(r == 0);
    CHECK(calls == 0);

    /* && : the right side is evaluated */
    r = one && ({ int t = f(5); t + 1; });
    CHECK(r == 1);
    CHECK(calls == 1);

    /* || : the right side is not evaluated */
    calls = 0;
    r = one || ({ int t = f(1); t; });
    CHECK(r == 1);
    CHECK(calls == 0);

    /* || : the right side is evaluated */
    r = zero || ({ int t = f(0); t; });
    CHECK(r == 0);
    CHECK(calls == 1);

    /* the guard protects the access */
    int value = 42;
    int* p = 0;
    r = p && ({ int t = *p; t; });
    CHECK(r == 0);
    p = &value;
    r = p && ({ int t = *p; t; });
    CHECK(r == 1);

    /* ?: : only the chosen branch runs */
    calls = 0;
    int v = zero ? ({ int t = f(1); t; }) : ({ int t = f(2); t; });
    CHECK(v == 2);
    CHECK(calls == 1);

    calls = 0;
    v = one ? ({ int t = f(10); t; }) : ({ int t = f(20); t; });
    CHECK(v == 10);
    CHECK(calls == 1);

    /* ?: with a hoisted branch and a plain one */
    calls = 0;
    v = one ? 7 : ({ int t = f(9); t; });
    CHECK(v == 7);
    CHECK(calls == 0);

    /* a ?: b, the temporary has the type of the result */
    long long big = id(0) ?: ({ long long t = 5000000000LL; t; });
    CHECK(big == 5000000000LL);
    double d = id(0) ?: ({ double t = 1.5; t; });
    CHECK(d == 1.5);
    int e = id(3) ?: ({ int t = f(4); t; });
    CHECK(e == 3);

    /* two statement expressions in one expression */
    int z = ({ int a = 1; a; }) + ({ int b = 2; b + 0; });
    CHECK(z == 3);

    int y = ({ int a = 4; a; }) * ({ int b = 5; b; }) + ({ int c = 6; c; });
    CHECK(y == 26);

    return failures;
}
