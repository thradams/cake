#pragma safety enable

void* _Owner _Opt calloc(int n, unsigned long size);
void free(void* _Owner _Opt ptr);

struct X {
    int i;
};

struct X* g;
void f(struct X* p)
{
    g = p;

    //cannot track the lifetime of the pointed object
#pragma cake diagnostic check "-E128"

}

int main()
{
    struct X* _Opt _Owner p = calloc(1, sizeof * p);
    if (p)
    {
        f(p);
        free(p);
        /*
          one object struct X was deleted
        */
    }
    if (g)
    {
        /*
          g points to one struct X
          programmer needs to assert(g != p);
        */
        g->i = 1;
    }
}
