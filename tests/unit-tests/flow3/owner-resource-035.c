
#pragma safety enable



void free( void* _Owner ptr);
_Uninitialized void* _Nonnull _Owner malloc(int size);
struct X { char * _Nonnull _Owner text; };

void x_destroy(_Dtor struct X*  p)
{
    free(p->text);
}

void x_delete(struct X* _Owner _Opt p)
{
    if (p)
    {
        x_destroy(p);
        /* FIXED: free takes void*, it cannot access (*p).text, so passing p only consumes p */
        free(p);
    }
}
