#include <stdio.h>
#include <stdlib.h>

/* -------------------------------------------------------
   1. Basic 1-D VLA
   ------------------------------------------------------- */
void fill_1d(int n)
{
    int a[n];          /* VLA */
    int i;
    for (i = 0; i < n; i++)
        a[i] = i * 2;

    for (i = 0; i < n; i++)
        printf("%d ", a[i]);
    printf("\n");
}

/* -------------------------------------------------------
   2. 2-D VLA  —  subscript rewriting a[i][j] → flat ptr
   ------------------------------------------------------- */
void fill_2d(int m, int n)
{
    int a[m][n];       /* 2-D VLA */
    int i, j;
    for (i = 0; i < m; i++)
        for (j = 0; j < n; j++)
            a[i][j] = i * n + j;

    for (i = 0; i < m; i++)
    {
        for (j = 0; j < n; j++)
            printf("%3d", a[i][j]);
        printf("\n");
    }
}

/* -------------------------------------------------------
   3. sizeof(VLA)  →  runtime size expression
   ------------------------------------------------------- */
void print_sizes(int n)
{
    int a[n];
    printf("sizeof(a)         = %zu\n"