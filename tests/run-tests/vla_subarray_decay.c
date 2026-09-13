/*
   Sub-arrays of a variably modified array used as values.

   `int a[n][m]` is flattened to `int *a`, so `a[1]` was emitted as the
   flat element `a[1 * __vm]` -- an `int`, not the `int[m]` sub-array.
   Every use that relies on array-to-pointer decay then miscompiled:
   `&a[1]` cast that int to a pointer, `int *r = a[1]` assigned an int,
   `*a[1]` dereferenced an int, `a[1] + k` was integer arithmetic and
   `f(a[1])` passed an int where a row was expected. The same happened
   through a pointer to a VM array (`int (*p)[m]`). Codegen now emits
   `&a[offset]`, which is exactly what the decay would have produced.
   Checked at runtime against direct indexing so a regression fails the
   run instead of only producing warnings in the generated C.
*/

#include <stdio.h>

static int failures = 0;

#define CHECK(cond) do { if (!(cond)) { printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); failures++; } } while(0)

static void take_row(int m, int r[m])
{
    r[0] = 77;
}

int main(void)
{
    int n = 2, m = 3;
    int a[n][m];

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            a[i][j] = i * 10 + j;
        }
    }

    /* address of a sub-array: pointer to row 1 */
    int (*pa1)[m] = &a[1];
    CHECK((char*)pa1 == (char*)a + m * sizeof(int));
    CHECK((*pa1)[2] == 12);

    /* sub-array decaying to int* */
    int *r = a[1];
    CHECK(r[0] == 10);
    CHECK(*a[1] == 10);
    CHECK(a[1] + 1 == &a[1][1]);
    CHECK(*(a[1] + 2) == 12);

    /* sub-array passed as a VM parameter */
    take_row(m, a[1]);
    CHECK(a[1][0] == 77);

    /* the same through a pointer to a VM array */
    int (*p)[m] = a;
    int (*pp1)[m] = &p[1];
    CHECK((char*)pp1 == (char*)a + m * sizeof(int));
    int (*pp0)[m] = &p[0];
    CHECK((char*)pp0 == (char*)a);
    int *r2 = p[1];
    CHECK(r2[1] == 11);
    CHECK(*p[1] == 77);
    take_row(m, p[0]);
    CHECK(a[0][0] == 77);

    /* three dimensions: sub-arrays at every level */
    int k = 2;
    int b[k][n][m];
    for (int x = 0; x < k; x++)
    {
        for (int y = 0; y < n; y++)
        {
            for (int z = 0; z < m; z++)
            {
                b[x][y][z] = x * 100 + y * 10 + z;
            }
        }
    }

    int (*pb1)[n][m] = &b[1];
    CHECK((char*)pb1 == (char*)b + n * m * sizeof(int));
    int (*pb2)[m] = &b[1][1];
    CHECK((char*)pb2 == (char*)b + (n * m + m) * sizeof(int));
    int *pb3 = &b[1][1][1];
    CHECK(*pb3 == 111);
    int *row = b[1][1];
    CHECK(row[2] == 112);

    int (*pb)[n][m] = b;
    int *row2 = pb[1][0];
    CHECK(row2[1] == 101);
    CHECK(sizeof(pb[1][0]) == m * sizeof(int));

    return failures;
}
