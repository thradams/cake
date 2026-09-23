#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void test_3d(int x, int y, int z)
{
    int (*p)[x][y][z] = malloc(sizeof *p);
    int i, j, k;
    for (i = 0; i < x; i++)
        for (j = 0; j < y; j++)
            for (k = 0; k < z; k++)
                (*p)[i][j][k] = i * 100 + j * 10 + k;

    printf("p[1][2][1] = %d\n", (*p)[1][2][1]); /* expect 121 */
    printf("p[2][0][3] = %d\n", (*p)[2][0][3]); /* expect 203 */
    free(p);
}

struct Point { int x; int y; };

void test_struct_array(int n)
{
    struct Point (*pts)[n] = malloc(sizeof *pts);
    int i;
    for (i = 0; i < n; i++)
    {
        (*pts)[i].x = i;
        (*pts)[i].y = i * 2;
    }
    for (i = 0; i < n; i++)
        printf("(%d,%d) ", (*pts)[i].x, (*pts)[i].y);
    printf("\n");
    free(pts);
}

void zero_row(int m, int (*row)[m])
{
    int j;
    for (j = 0; j < m; j++)
        (*row)[j] = 0;
}

void set_diagonal(int n, int m, int (*mat)[n][m])
{
    int i;
    for (i = 0; i < n && i < m; i++)
        (*mat)[i][i] = 1;
}

void test_call_chain(int n, int m)
{
    int (*mat)[n][m] = malloc(sizeof *mat);
    int i, j;
    /* zero everything */
    for (i = 0; i < n; i++)
        for (j = 0; j < m; j++)
            (*mat)[i][j] = 0;

    set_diagonal(n, m, mat);

    for (i = 0; i < n; i++)
    {
        for (j = 0; j < m; j++)
            printf("%d ", (*mat)[i][j]);
        printf("\n");
    }
    free(mat);
}

void test_jagged(int rows, int cols[rows])
{
    int i, j;

    /* array of pointers, each pointing to a VM row */
    int **jagged = malloc(sizeof(int*) * rows);
    for (i = 0; i < rows; i++)
    {
        int (*row)[cols[i]] = malloc(sizeof *row);
        for (j = 0; j < cols[i]; j++)
            (*row)[j] = i * 10 + j;
        jagged[i] = (int*)row;
    }

    for (i = 0; i < rows; i++)
    {
        for (j = 0; j < cols[i]; j++)
            printf("%3d", jagged[i][j]);
        free(jagged[i]);
        printf("\n");
    }
    free(jagged);
}

void* alloc_matrix(int n, int m)
{
    int (*p)[n][m] = malloc(sizeof *p);
    return p;
}

void test_return_vm(int n, int m)
{
    int (*mat)[n][m] = alloc_matrix(n, m);
    int i, j;
    for (i = 0; i < n; i++)
        for (j = 0; j < m; j++)
            (*mat)[i][j] = (i + 1) * (j + 1);
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < m; j++)
            printf("%4d", (*mat)[i][j]);
        printf("\n");
    }
    free(mat);
}

void test_sizeof_in_expr(int n, int m)
{
    int (*a)[n][m] = malloc(sizeof *a);
    int (*b)[n][m] = malloc(sizeof *b);

    int i, j;
    for (i = 0; i < n; i++)
        for (j = 0; j < m; j++)
            (*a)[i][j] = i * m + j + 1;

    /* memcpy using sizeof *a */
    memcpy(b, a, sizeof *a);

    /* memset using sizeof *b */
    memset(a, 0, sizeof *a);

    printf("a[0][0] after memset = %d\n", (*a)[0][0]); /* 0 */
    printf("b[0][0] after memcpy = %d\n", (*b)[0][0]); /* 1 */
    printf("b[n-1][m-1] = %d\n", (*b)[n-1][m-1]); /* n*m */
    free(a);
    free(b);
}

void test_conditional_size(int use_large)
{
    int n = use_large ? 10 : 3;
    int m = use_large ? 10 : 3;
    int (*p)[n][m] = malloc(sizeof *p);
    int i, j;
    for (i = 0; i < n; i++)
        for (j = 0; j < m; j++)
            (*p)[i][j] = i * m + j;
    printf("p[%d][%d] = %d\n", n-1, m-1, (*p)[n-1][m-1]);
    printf("sizeof = %zu\n", sizeof(*p));
    free(p);
}

void scale(int n, int m, int (*mat)[n][m], int factor)
{
    int i, j;
    for (i = 0; i < n; i++)
        for (j = 0; j < m; j++)
            (*mat)[i][j] *= factor;
}

void add_matrices(int n, int m,
    int (*dst)[n][m],
    int (*src)[n][m])
{
    int i, j;
    for (i = 0; i < n; i++)
        for (j = 0; j < m; j++)
            (*dst)[i][j] += (*src)[i][j];
}

void test_matrix_ops(int n, int m)
{
    int (*a)[n][m] = malloc(sizeof *a);
    int (*b)[n][m] = malloc(sizeof *b);
    int i, j;

    for (i = 0; i < n; i++)
        for (j = 0; j < m; j++)
        {
            (*a)[i][j] = i * m + j + 1;
            (*b)[i][j] = 1;
        }

    scale(n, m, a, 2);
    add_matrices(n, m, a, b);

    /* a[i][j] = (i*m+j+1)*2 + 1 */
    printf("a[0][0]=%d\n", (*a)[0][0]); /* (0+0+1)*2+1 = 3 */
    printf("a[1][1]=%d\n", (*a)[1][1]); /* (m+2)*2+1   */
    free(a);
    free(b);
}

void test_ptr_arith(int n)
{
    int (*p)[n] = malloc(sizeof(int) * n * 3);
    int (*q)[n];
    int i;

    /* fill 3 rows */
    for (i = 0; i < n * 3; i++)
        ((int*)p)[i] = i;

    /* advance pointer by one "row" of n ints */
    q = p + 1;

    printf("p[0]=%d q[0]=%d\n", (*p)[0], (*q)[0]); /* 0, n */
    free(p);
}

void test_vm_typedef(int n, int m)
{
    /* C99: typedef int Row[m]; Row (*mat)[n] = malloc(...) */
    /* Equivalent without typedef: */
    int (*mat)[n][m] = malloc(sizeof *mat);
    int i, j;
    for (i = 0; i < n; i++)
        for (j = 0; j < m; j++)
            (*mat)[i][j] = (i == j) ? 1 : 0; /* identity */

    for (i = 0; i < n; i++)
    {
        for (j = 0; j < m; j++)
            printf("%d ", (*mat)[i][j]);
        printf("\n");
    }
    free(mat);
}

int main(void)
{
    printf("=== 1. 3-D VM pointer ===\n");
    test_3d(3, 3, 4);

    printf("=== 2. VM pointer to struct array ===\n");
    test_struct_array(4);

    printf("=== 3. VM through call chain ===\n");
    test_call_chain(3, 4);

    printf("=== 4. Jagged array via VM rows ===\n");
    int cols[4] = {2, 4, 1, 3};
    test_jagged(4, cols);

    printf("=== 5. VM via void* return ===\n");
    test_return_vm(3, 4);

    printf("=== 6. sizeof in memcpy/memset ===\n");
    test_sizeof_in_expr(3, 4);

    printf("=== 7. Conditional allocation ===\n");
    test_conditional_size(0);
    test_conditional_size(1);

    printf("=== 8. Matrix ops with multiple VM params ===\n");
    test_matrix_ops(2, 3);

    printf("=== 9. VM pointer arithmetic ===\n");
    test_ptr_arith(4);

    printf("=== 10. VM typedef equivalent ===\n");
    test_vm_typedef(3, 3);

    return 0;
}
