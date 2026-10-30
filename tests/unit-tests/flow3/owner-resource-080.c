#pragma safety enable



_Clear void* _Nonnull _Owner calloc(unsigned n, unsigned size);
void free(void* _Owner ptr);

struct X {
    char* _Owner name;
};

struct X* _Owner F(int i)
{
    struct X* _Owner p1 = 0;
    struct X* _Owner p2 = calloc(1, sizeof * p2);
    if (p2)
    {
        p1 = p2;
    }
    return p1;
}
