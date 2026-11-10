struct X { int a[2]; };

int* f(int c)
{
    int* p = c ? (int[3]){1, 2, 3} : (int[6]){0};
    p[4]; //lint 70 /* past (int[3]) only */
    p[7]; //lint 70

    struct X* x = &(struct X){0};
    int* q = x->a;
    q[4]; //lint 70

    return p; //lint 31
}

int* g(void)
{
    int* p = 0;
    {
        p = (int[1]){1};
    }
    return p; //lint 31
}
