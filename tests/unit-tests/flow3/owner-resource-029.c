#pragma safety enable



struct X { int i; void* p; };
_Clear void* _Owner calloc(int i, int sz);
void free(void* _Owner _Opt p);

int main()
{
    struct X* _Owner p = calloc(1, sizeof * p);
    if (p)
    {
    compile_assert(p->i == 0);
    compile_assert(p->p == 0);
    }
    free(p);
}
