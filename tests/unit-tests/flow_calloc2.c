#pragma safety enable



struct X { int i; void* _Opt p; };
void* _Owner _Opt calloc(int i, int sz);
void free(void* _Owner _Opt p);

int main()
{
    struct X* _Owner _Opt p = calloc(1, 1);
    assert_state(p, "null | not-null ");
    if (p)
    {
    assert_state(p->i, "zero");
    assert_state(p->p, "null");
    }
    free(p);
}
