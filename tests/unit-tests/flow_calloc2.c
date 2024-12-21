#pragma safety enable



struct X { int i; void* _Opt p; };
void* _Owner _Opt calloc(int i, int sz);
void free(void* _Owner _Opt p);

int main()
{
    struct X* _Owner _Opt p = calloc(1, 1);
    static_state(p, "null | not-null ");
    if (p)
    {
    static_state(p->i, "zero");
    static_state(p->p, "null");
    }
    free(p);
}
