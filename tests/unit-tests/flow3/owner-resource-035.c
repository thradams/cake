
#pragma safety enable



void free( void* _Owner ptr);
void* _Owner _Uninitialized malloc(int size);
struct X { char * _Owner text; };

void x_destroy(_Dtor struct X*  p)
{
    free(p->text);
}

void x_delete(struct X* _Owner _Opt p)
{
    if (p)
    {
        x_destroy(p);
        /* FIXED: this used to warn "object '(*p).text' lifetime has
           ended" here, even though free's parameter is void* -- a
           type-erased pointer through which the callee cannot access
           any member of *p at all. Passing p to it is just consuming p
           ITSELF (already correctly tracked separately), not an access
           to (*p).text's already-ended state. */
        free(p);
    }
}
