#pragma safety enable

/* an ENDED fact whose origin is an ancestor of the current map must be seen through an alias (p->pY freed via pY) */

void* _Owner _Opt _Clear calloc(unsigned int n, unsigned long size);
void free(void* _Owner _Opt ptr);

struct Y {
    int i;
};

struct X {
    int i;
    struct Y* _Opt pY;
};

/* must warn: pX->pY was freed through the pY alias, p->pY reaches the dead object */
int aliased_free_not_seen_through_alias(void)
{
    struct X* _Owner _Opt pX = calloc(1, sizeof *pX);
    if (pX)
    {
        struct Y* _Owner _Opt pY = calloc(1, sizeof *pY);
        if (pY)
        {
            pX->pY = pY;
            struct X* _Opt p = pX;
            free(pY);
            p->pY->i = 1; /* warns: p->pY's pointee lifetime has ended */ //lint 31
        }
        free(pX);
    }
    return 0;
}

/* Must NOT warn: no free happened, p->pY is still live. */
void aliased_no_free_is_clean(void)
{
    struct X* _Owner _Opt pX = calloc(1, sizeof *pX);
    if (pX)
    {
        struct Y* _Owner _Opt pY = calloc(1, sizeof *pY);
        if (pY)
        {
            pX->pY = pY;
            struct X* _Opt p = pX;
            p->pY->i = 1;
            free(pY);
        }
        free(pX);
    }
}
