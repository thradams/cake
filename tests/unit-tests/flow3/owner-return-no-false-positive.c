#pragma safety enable

/* an owner returned to the caller is MOVED and must not be re-marked ENDED at scope exit (no false "owner not moved") */

_Uninitialized void* _Nonnull _Owner malloc(unsigned long size);

int* _Nonnull _Owner f_return_owner(void)
{
    int* _Owner p = malloc(sizeof(int));
    return p; /* moves p to the caller: must NOT warn "not moved" */
}

int* _Owner f_return_owner_opt(void)
{
    int* _Owner p = malloc(sizeof(int));
    return p; /* same, with an optional owner */
}
