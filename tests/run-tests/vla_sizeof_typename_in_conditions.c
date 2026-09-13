/*
   VM type-names (`sizeof(int[n])`, `_Countof(int[n])`) and VM typedefs.

   1. The dimension of a type-name used to be snapshotted into a hoisted
      `__vmN = n;` statement placed before the enclosing statement. Inside
      an `if`/`switch`/loop condition the hoisted statement was dropped
      (the body's first block item clears the pending statements), so the
      generated C read an uninitialized `__vmN`. A type-name's dimension is
      now emitted inline, where C evaluates it anyway.

   2. A VM typedef's snapshot was tracked by text search in the CURRENT
      block's declarations, so a nested block (including the do/while of a
      macro like CHECK) re-declared and re-assigned the snapshot, and
      `sizeof(T)` saw the new value of n instead of the one captured when
      the typedef was declared.

   3. Statements hoisted by an `if` condition (the old-value temp of a
      VM `p++`) were lost the same way as in 1.
*/

#include <stdio.h>

static int failures = 0;

#define CHECK(cond) do { if (!(cond)) { printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); failures++; } } while(0)

int main(void)
{
    int n = 2, m = 3;

    /* type-name inside macro/do-while and if conditions */
    CHECK(sizeof(int[n][m]) == 6 * sizeof(int));
    CHECK(_Countof(int[n]) == 2);

    if (sizeof(int[n]) != 2 * sizeof(int))
    {
        printf("FAIL %s:%d\n", __FILE__, __LINE__);
        failures++;
    }

    /* type-name inside loop conditions: re-evaluated every iteration */
    int cnt = 0;
    while (cnt < (int)sizeof(int[n]))
    {
        cnt++;
    }
    CHECK(cnt == 8);

    for (cnt = 0; cnt < (int)sizeof(int[m]); cnt++)
    {
    }
    CHECK(cnt == 12);

    cnt = 0;
    do
    {
        cnt++;
    }
    while (cnt < (int)sizeof(char[n]));
    CHECK(cnt == 2);

    /* the dimension must follow n when n changes between evaluations */
    cnt = 0;
    int dim = 1;
    while (dim < 4)
    {
        cnt += (int)sizeof(char[dim]);
        dim++;
    }
    CHECK(cnt == 1 + 2 + 3);

    /* type-name in a switch controlling expression */
    switch (sizeof(int[n]))
    {
        case 8: cnt = 1; break;
        default: cnt = 0; break;
    }
    CHECK(cnt == 1);

    /* VM typedef: size captured at the typedef, not at each use */
    typedef int T[n];
    n = 100;
    CHECK(sizeof(T) == 2 * sizeof(int));
    {
        CHECK(sizeof(T) == 2 * sizeof(int));
        {
            CHECK(_Countof(T) == 2);
        }
    }
    T t;
    CHECK(sizeof(t) == 2 * sizeof(int));
    n = 2;

    /* VLA object: size captured at the declaration */
    int q = 4;
    int v[q];
    q = 1000;
    CHECK(sizeof(v) == 4 * sizeof(int));
    CHECK(_Countof(v) == 4);

    /* VLA with the empty initializer (memset path, no compile-time sizeof) */
    int a[n][m] = {};
    CHECK(a[1][2] == 0);

    /* VM pointer postfix ++ inside an if condition */
    int (*p)[m] = a;
    if ((char*)p++ == (char*)a)
    {
        CHECK((char*)p == (char*)a + m * sizeof(int));
    }
    else
    {
        printf("FAIL %s:%d\n", __FILE__, __LINE__);
        failures++;
    }

    return failures;
}
