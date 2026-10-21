#pragma safety enable

/* a const object of our own (by-value parameter, local) still moves its owners at the end of its lifetime; only const through a pointer forbids it */

void free(void* _Owner _Opt p);
void* _Owner _Opt malloc(unsigned long size);

struct X { char* _Owner _Opt p; };

void by_value(const struct X x)
{
    free(x.p);
}

void const_local_owner(void)
{
    char* _Owner _Opt const p = malloc(1);
    free(p);
}

void through_pointer(const struct X* x)
{
    char* _Owner _Opt p = x->p; //lint 99
    free(p);
} //lint 72