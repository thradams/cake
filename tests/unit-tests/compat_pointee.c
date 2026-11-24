/* flow.md A15.2: through a pointer, _Nullable must match (mutable), may be added (const), may be removed (_Out) */
#pragma safety enable

void get(char * _Out _Owner _Nullable * p);
void get2(char * _Out _Owner * p);
void free(void * _Owner _Nullable p);

void f(int * p, int * _Nullable q)
{
    int * _Nullable * pp1 = &p;
    int ** pp2 = &q;
    int * _Nullable const * pp4 = &p;
    int * const * pp5 = &q;
}

void g()
{
    char * _Owner s;
    get(&s);
    free(s);

    char * _Owner s2;
    get2(&s2);
    free(s2);
}
