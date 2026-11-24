/* flow.md A15.1: the copied value may drop _Nullable and _Owner, but not gain _Owner */
#pragma safety enable

void free(void * _Owner _Nullable p);

void f(int * p, int * _Nullable q, int * _Owner o)
{
    int * a = q;
    int * b = o;
    int * _Owner c = p; //lint 78
    free(c);
    free(o);
}
