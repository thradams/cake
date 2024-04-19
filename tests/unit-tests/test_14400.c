struct X { int i; void* p; };
void* _Owner calloc(int i, int sz);
void free(void* _Owner p);

int main()
{
    struct X* _Owner p = calloc(1, 1);
    static_state(p, "maybe-null ");
    static_state(p->i, "zero");
    static_state(p->p, "null");
    free(p);
}
