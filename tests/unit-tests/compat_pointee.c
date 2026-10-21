/* flow.md A15.2: through a pointer, _Opt must match (mutable), may be added (const), may be removed (_Out) */
#pragma safety enable

void get(char * _Out _Owner _Opt * p);
void get2(char * _Out _Owner * p);
void free(void * _Owner _Opt p);

void f(int * p, int * _Opt q)
{
    int * _Opt * pp1 = &p; //lint 54
    int ** pp2 = &q; //lint 54
    int * _Opt const * pp4 = &p;
    int * const * pp5 = &q; //lint 54
}

void g()
{
    char * _Owner s;
    get(&s); //lint 54
    free(s);

    char * _Owner _Opt s2;
    get2(&s2);
    free(s2);
}
