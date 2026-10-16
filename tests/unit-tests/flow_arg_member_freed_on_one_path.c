#pragma safety enable

// a member released on only one path: passing the struct to a _Dtor reads a released pointer.

struct X { int* _Owner _Opt p; };

void* _Owner _Opt malloc(unsigned long n);
void free(void* _Owner _Opt p);
void destroy(_Dtor struct X* x);

void f(int c)
{
    struct X x = { 0 };
    x.p = malloc(sizeof(int));
    if (c)
        free(x.p);

    destroy(&x); //lint 32
}
