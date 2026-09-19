#pragma safety enable

/* sizeof / _Alignof / _Countof are seeded as unsigned constants for compile_assert; a VLA's sizeof stays unknown */

void scalar_sizes(void)
{
    compile_assert(sizeof(char) == 1);
    compile_assert(sizeof(int) == 4);
    compile_assert(sizeof(double) == 8);
}

void array_size(void)
{
    int v[10];
    compile_assert(sizeof(v) == 40);       /* 10 * sizeof(int) */
}

void alignment(void)
{
    compile_assert(_Alignof(int) == 4);
}

/* a VLA's sizeof is a runtime value, not provable */
void vla(int n)
{
    int a[n];
    unsigned long s = sizeof(a);
    (void)s;
}
