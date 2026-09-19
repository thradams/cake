#pragma safety enable

/* erasing a struct to void* (free) checks every _Owner member first: .surname was never released and must be reported even though nothing reads it */

struct X
{
    int* _Owner _Opt name;
    int* _Owner _Opt surname;
};

void free(void* _Owner _Opt p);

/* Erasure through a function parameter (e.g. free()). */
void delete1(struct X* _Owner _Opt p)
{
    if (p)
    {
        free(p->name);
        /* forgot to free p->surname */
        free(p); //lint 29 owner object (.surname) not moved (opt-mem-nonnull see line 40)
    }
}

/* Erasure through an assignment/initialization. */
void assign1(struct X* _Owner _Opt p)
{
    if (p)
    {
        free(p->name);
        /* forgot to free p->surname */
        void* _Owner _Opt v = p; //lint 29 owner object (.surname) not moved (opt-mem-nonnull see line 51)
        free(v); //lint 29 owner object (.surname) not moved (opt-mem-nonnull see line 51)
    }
}

/* Erasure through a return. */
void* _Owner _Opt detach1(struct X* _Owner _Opt p)
{
    if (p)
    {
        free(p->name);
        /* forgot to free p->surname */
        return p; //lint 29 owner object (.surname) not moved (opt-mem-nonnull see line 62)
    }
    return 0;
}
