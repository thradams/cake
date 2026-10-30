#pragma safety enable


_Clear void* _Owner calloc(int n, int size);

struct X {
    char* _Owner name;
};

void* _Owner f1()
{
    struct X* _Owner p = calloc(1, sizeof(struct X));
    if (p)
    {
        /* no discard warning: calloc is _Clear, p->name is known zero */
        p->name = calloc(1, 2);
    }

    //
    return p; //lint 29 owner object (.name) not moved (call-opt-nonnull see line 17)
}
