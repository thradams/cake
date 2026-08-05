#pragma safety enable


void* _Owner _Opt _Clear calloc(int n, int size);

struct X {
    char* _Owner _Opt name;
};

void* _Owner _Opt f1()
{
    struct X* _Owner _Opt p = calloc(1, sizeof(struct X));
    if (p)
    {
        /* No discard warning: calloc is declared _Clear above, so flow3 knows
           p->name is zero here rather than an owner of unknown state. */
        p->name = calloc(1, 2);
    }

    //
    return p; //lint 29 owner object (.name) not moved (call-opt-nonnull see line 17)
}
