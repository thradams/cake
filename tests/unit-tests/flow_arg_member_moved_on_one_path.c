#pragma safety enable

// a member moved on only one path is still possibly moved when the struct is passed by pointer.

struct X { int* _Owner _Opt p; };

void* _Owner _Opt malloc(unsigned long n);
void sink(int* _Owner _Opt p);
void use(const struct X* x);
void destroy(_Dtor struct X* x);

void f(int c)
{
    struct X x = { 0 };
    x.p = malloc(sizeof(int));
    if (c)
        sink(x.p);

    use(&x); //lint 32
    destroy(&x); //lint 32
}
