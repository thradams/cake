void* _Owner calloc(unsigned n, unsigned size);
void free(void* _Owner ptr);

struct X {
    char* _Owner name;
};

struct X* _Owner F(int i)
{
    struct X* _Owner p1 = 0;

    switch (i)
    {
    case 1:
        struct X* _Owner p2 = calloc(1, sizeof * p2);
        if (p2)
        {
            static_debug(p2);
            static_set(*p2, "zero");
            p1 = p2;
            static_debug(p1);
        }
        break;
    case 2:
        break;
    }
    static_debug(p1);
    return p1;
}
