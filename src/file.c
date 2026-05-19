#include <stdio.h>
#include <stdlib.h>

void f(int n, int m, int (*p)[n][m])
{
    for (int i = 0; i < n; i++)
    {

        for (int j = 0; j < m; j++)
        {
            printf("%d ", (*p)[i][j]);

        }
        printf("\n");
    }
    printf("\n");
}

void f2(int n, int m, int a[n][m])
{
    for (int i = 0; i < n; i++)
    {

        for (int j = 0; j < m; j++)
        {
            printf("%d ", a[i][j]);

        }
        printf("\n");
    }
    printf("\n");
}

int main(void)
{
    int a[2][3] = {
         1, 2,
         3, 4,
         5, 6
    };
    f(2, 3, &a);


    f2(2, 3, a);

    int n = 2, m = 3;
    int (*p)[n][m] = &a;
    p = &a;
    f(2, 3, p);
}