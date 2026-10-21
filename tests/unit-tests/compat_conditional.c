/* flow.md A15.8: ?: leaves the value's _Opt to flow; pointees must match */
#pragma safety enable

void free(void * _Owner _Opt p);

void f1(int * _Opt p1, int * q)
{
    int * p2 = p1 ? p1 : 0; //lint 35
    int * p3 = p1 ? p1 : q;
}

void f2(int * _Owner _Opt p1)
{
    int * _Owner _Opt p = p1 ? p1 : 0;
    free(p);
}

void f3(int * _Opt * pp1, int ** pp2, int c)
{
    int * _Opt * pp = c ? pp1 : pp1;
    int * _Opt * pp3 = c ? pp2 : pp1; //lint 54
}
