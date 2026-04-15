void printf(const char* s, ...);

void print1D(int n, int a[n])
{
    for (int i = 0; i < n; i++)
    {
        printf(" %d", a[i]);
    }
    printf("\n");
}
