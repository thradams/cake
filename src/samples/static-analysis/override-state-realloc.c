#pragma safety enable

void* _Owner _Opt realloc(void* _Opt ptr, unsigned size);
void* _Owner _Opt _Uninitialized malloc(unsigned long size);
void free(void* _Owner _Opt ptr);

void f()
{
    void * _Owner _Opt p = malloc(1);
    void * _Owner _Opt p2 = realloc(p, 2);
    if (p2 != 0)
    {
        p = p2; //lint26 (semantic of realloc)
    }
    free(p);
}
