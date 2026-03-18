
#include <stdio.h>
#include <stdlib.h>

void print2D(int n, int m, int a[n][m])
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            printf("%d ", a[i][j]);
        }
        printf("\n");
    }
}

void print1D(int n, int a[n])
{
    for (int i = 0; i < n; i++)
    {
        printf(" %d", a[i]);
    }
    printf("\n");
}

void test_1d(int n)
{
    int (*p)[n] = malloc(sizeof * p);
    int i;
    for (i = 0; i < n; i++)
        (*p)[i] = i + 1;
    for (i = 0; i < n; i++)
        printf("%d ", (*p)[i]);
    printf("\n");
    free(p);
}

void test_2d(int n, int m)
{
    int (*p)[n][m] = malloc(sizeof * p);
    int i, j;
    for (i = 0; i < n; i++)
        for (j = 0; j < m; j++)
            (*p)[i][j] = i * m + j;
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < m; j++)
            printf("%3d", (*p)[i][j]);
        printf("\n");
    }
    free(p);
}

void test_sizeof_stable(int n, int m)
{
    int (*p)[n][m] = malloc(sizeof * p);
    size_t sz = sizeof(*p);
    printf("sizeof(*p) = %zu\n", sz);
    n = 999;
    m = 999;
    printf("sizeof(*p) after n=999,m=999 = %zu\n", sizeof(*p));
    printf("same? %d\n", sz == sizeof(*p));
    free(p);
}

void fill(int n, int m, int (*grid)[n][m])
{
    int i, j;
    for (i = 0; i < n; i++)
        for (j = 0; j < m; j++)
            (*grid)[i][j] = i * 10 + j;
}

void print_grid(int n, int m, int (*grid)[n][m])
{
    int i, j;
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < m; j++)
            printf("%3d", (*grid)[i][j]);
        printf("\n");
    }
}

void test_vm_param(int n, int m)
{
    int (*p)[n][m] = malloc(sizeof * p);
    fill(n, m, p);
    print_grid(n, m, p);
    free(p);
}

void test_multiple(int n, int m)
{
    int (*a)[n] = malloc(sizeof * a);
    int (*b)[m] = malloc(sizeof * b);
    int i;
    for (i = 0; i < n; i++) (*a)[i] = i;
    for (i = 0; i < m; i++) (*b)[i] = i + 100;
    printf("a[0]=%d a[n-1]=%d\n", (*a)[0], (*a)[n - 1]);
    printf("b[0]=%d b[m-1]=%d\n", (*b)[0], (*b)[m - 1]);
    free(a);
    free(b);
}

void test_mixed_dims(int n)
{
    int (*p)[4][n] = malloc(sizeof * p);
    int i, j;
    for (i = 0; i < 4; i++)
        for (j = 0; j < n; j++)
            (*p)[i][j] = i * n + j;
    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < n; j++)
            printf("%3d", (*p)[i][j]);
        printf("\n");
    }
    free(p);
}

void test_reassign(int n)
{
    int (*p)[n] = malloc(sizeof * p);
    printf("sizeof(*p) first  = %zu\n", sizeof(*p));
    free(p);
    n = 2;                         /* n changes */
    p = malloc(sizeof * p);         /* new allocation — but snapshot is fixed */
    printf("sizeof(*p) second = %zu\n", sizeof(*p));
    free(p);
}

int main(void)
{
    printf("=== 1. 1-D VM pointer ===\n");
    test_1d(5);

    printf("=== 2. 2-D VM pointer ===\n");
    test_2d(3, 4);

    printf("=== 3. sizeof stability ===\n");
    test_sizeof_stable(3, 4);

    printf("=== 4. VM parameter ===\n");
    test_vm_param(3, 4);

    printf("=== 5. Multiple VM pointers ===\n");
    test_multiple(4, 3);

    printf("=== 6. Mixed constant/runtime dims ===\n");
    test_mixed_dims(3);

    printf("=== 7. Reassignment ===\n");
    test_reassign(5);

    return 0;
}

