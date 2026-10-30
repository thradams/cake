#pragma safety enable

void* _Owner realloc(void* _Nullable ptr, unsigned size);
_Uninitialized void* _Owner malloc(unsigned long size);
void free(void* _Owner _Nullable ptr);

void f()
{
    void * _Owner p = malloc(1);
    void * _Owner p2 = realloc(p, 2);
    if (p2 != 0)
    {
        p = p2; //lint26 (semantic of realloc)
    }
    free(p);
}
