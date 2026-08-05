#pragma safety enable



struct X { int i; void* _Opt p; };
void* _Owner _Opt _Clear calloc(int i, int sz);
void free(void* _Owner _Opt p);

int main()
{
    struct X* _Owner _Opt p = calloc(1, 1);
    if (p)
    {
    compile_assert(p->i == 0);
    compile_assert(p->p == 0);
    }
    free(p);
}
