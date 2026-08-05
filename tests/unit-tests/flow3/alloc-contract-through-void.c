#pragma safety enable

void* _Owner _Opt _Clear calloc(unsigned long n, unsigned long size);
void free(void* _Owner _Opt ptr);

struct X { int i; int j; };

void init_direct(void)
{
    struct X* _Owner _Opt x = calloc(1, sizeof * x);
    if (x == 0) return;
    compile_assert(x->i == 0);
    compile_assert(x->j == 0);
    free(x);
}
