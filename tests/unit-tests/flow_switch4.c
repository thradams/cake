#pragma safety enable


void* _Owner calloc(unsigned n, unsigned size);
void free(void* _Owner ptr);

struct X {
    char* _Owner _Opt name;
};

struct X* _Owner _Opt F(int i)
{
    struct X* _Owner _Opt p1 = 0;

    switch (i)
    {
        case 1:
            struct X* _Owner p2 = calloc(1, sizeof * p2);
            if (p2)
            {
              static_set(*p2, "zero");
                p1 = p2;
            }
            break;
        case 2:
            break;
    }

    return p1;
}