/*
   github.com/thradams/cake/issues/423

   `int a[n][m]` is flattened to a plain `int *a` by its own declaration
   codegen, so `a[i][j]` -> `a[j + i*m]` just works. `int (*p)[m] = a;` is
   NOT flattened the same way -- it keeps its `int (*p)[]` shape (pointer to
   an INCOMPLETE array type, dimension erased) -- so emitting `p[i][j]` as
   the same flat `p[j + i*m]` indexes through an incomplete array type,
   which isn't legal C and silently miscompiled. Checked at runtime against
   values written through `a` directly, so a codegen regression that goes
   back to indexing `p` unflattened fails here instead of only failing to
   compile (real compilers differ on whether they even reject it).
*/
#include <stdio.h>

static int failures = 0;

#define CHECK(cond) do { if (!(cond)) { printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); failures++; } } while(0)

static int test_nested_vm(int n, int m)
{
    int a[n][m];

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            a[i][j] = i * 10 + j;
        }
    }

    int (*p)[m] = a;

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            CHECK(p[i][j] == a[i][j]);
            CHECK(p[i][j] == i * 10 + j);
        }
    }

    /* writing through p must reach the same storage as a */
    p[1][1] = 999;
    CHECK(a[1][1] == 999);

    return failures;
}

int main(void)
{
    return test_nested_vm(3, 4);
}
