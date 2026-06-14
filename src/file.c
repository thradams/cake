
#pragma safety enable

void* _Owner _Opt malloc(unsigned long size);
void free(void* _Owner _Opt ptr);

struct X {
    int a, b;
};
void f(int c)
{
    struct X* p;
    p = malloc(sizeof(struct X));
    if (p)
    {
        static_debug(*p);
    }
   // static_debug(0);

}

