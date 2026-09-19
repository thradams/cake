#pragma safety enable

/* an owner returned to the caller is MOVED and must not be re-marked ENDED at scope exit (no false "owner not moved") */

void* _Owner _Uninitialized malloc(unsigned long size);

int* _Owner f_return_owner(void)
{
    int* _Owner p = malloc(sizeof(int));
    return p; /* moves p to the caller: must NOT warn "not moved" */
}

int* _Owner _Opt f_return_owner_opt(void)
{
    int* _Owner _Opt p = malloc(sizeof(int));
    return p; /* same, with an optional owner */
}
