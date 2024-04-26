#pragma safety enable



void* _Owner calloc(unsigned n, unsigned size);
void free(void* _Owner ptr);

struct X {
    char* _Owner _Opt name;
};

struct X* _Owner _Opt F(int i)
{
    struct X* _Owner _Opt p1 = 0;
    struct X* _Owner _Opt p2 = calloc(1, sizeof * p2);
    if (p2)
    {
        p1 = p2;
    }
    return p1;
}
