/* flow.md A15.8: ?: leaves the value's _Nullable to flow; pointees must match */
#pragma safety enable

void free(void * _Owner _Nullable p);

void f1(int * _Nullable p1, int * q)
{
    int * p2 = p1 ? p1 : 0;
    int * p3 = p1 ? p1 : q;
}

void f2(int * _Owner _Nullable p1)
{
    int * _Owner p = p1 ? p1 : 0;
    free(p);
}

void f3(int * _Nullable * pp1, int * _Nonnull * pp2, int c)
{
    int * _Nullable * pp = c ? pp1 : pp1;
    int * _Nullable * pp3 = c ? pp2 : pp1; //lint 54
}
